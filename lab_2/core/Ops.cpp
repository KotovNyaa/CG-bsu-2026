#include "Ops.h"
#include "../lib/ColorMath.h"
#include <algorithm>
#include <cmath>
#include <vector>

void Ops::computeHist(const QImage &img, HistData &data) {
  data.r.assign(256, 0);
  data.g.assign(256, 0);
  data.b.assign(256, 0);
  data.gray.assign(256, 0);

  uint64_t sum = 0;
  data.stats.min = 255;
  data.stats.max = 0;

  const int total = img.width() * img.height();
  for (int y = 0; y < img.height(); ++y) {
    const auto *line = img.constScanLine(y);
    for (int x = 0; x < img.width(); ++x) {
      const uchar r = line[x * 3];
      const uchar g = line[x * 3 + 1];
      const uchar b = line[x * 3 + 2];
      const uchar yVal = static_cast<uchar>(0.299 * r + 0.587 * g + 0.114 * b);

      data.r[r]++;
      data.g[g]++;
      data.b[b]++;
      data.gray[yVal]++;

      sum += yVal;
      if (yVal < data.stats.min) data.stats.min = yVal;
      if (yVal > data.stats.max) data.stats.max = yVal;
    }
  }
  data.stats.avg = total > 0 ? static_cast<double>(sum) / total : 0.0;
}

QImage Ops::linearStretch(const QImage &src, int minVal, int maxVal) {
  if (minVal >= maxVal) return src;

  uchar lut[256];
  for (int i = 0; i < 256; ++i) {
    lut[i] = static_cast<uchar>(std::clamp(((i - minVal) * 255) / (maxVal - minVal), 0, 255));
  }

  QImage dst(src.size(), src.format());
  for (int y = 0; y < src.height(); ++y) {
    const auto *sLine = src.constScanLine(y);
    auto *dLine = dst.scanLine(y);
    for (int x = 0; x < src.width() * 3; ++x) {
      dLine[x] = lut[sLine[x]];
    }
  }
  return dst;
}

static std::vector<uchar> makeCdfLut(const std::vector<uint32_t> &hist, int total) {
  std::vector<uchar> lut(256, 0);
  uint32_t cdf = 0;
  uint32_t cdfMin = 0;

  for (int i = 0; i < 256; ++i) {
    if (hist[i] > 0) {
      cdfMin = hist[i];
      break;
    }
  }

  if (total <= static_cast<int>(cdfMin)) return lut;

  for (int i = 0; i < 256; ++i) {
    cdf += hist[i];
    lut[i] = static_cast<uchar>(std::clamp(
        std::round(((static_cast<double>(cdf) - cdfMin) / (total - cdfMin)) * 255.0),
        0.0, 255.0));
  }
  return lut;
}

QImage Ops::equalizeRgb(const QImage &src) {
  HistData hist;
  computeHist(src, hist);
  const int total = src.width() * src.height();

  const auto lutR = makeCdfLut(hist.r, total);
  const auto lutG = makeCdfLut(hist.g, total);
  const auto lutB = makeCdfLut(hist.b, total);

  QImage dst(src.size(), src.format());
  for (int y = 0; y < src.height(); ++y) {
    const auto *sLine = src.constScanLine(y);
    auto *dLine = dst.scanLine(y);
    for (int x = 0; x < src.width(); ++x) {
      dLine[x * 3] = lutR[sLine[x * 3]];
      dLine[x * 3 + 1] = lutG[sLine[x * 3 + 1]];
      dLine[x * 3 + 2] = lutB[sLine[x * 3 + 2]];
    }
  }
  return dst;
}

QImage Ops::equalizeHls(const QImage &src) {
  const int total = src.width() * src.height();
  std::vector<uint32_t> lHist(256, 0);

  for (int y = 0; y < src.height(); ++y) {
    const auto *line = src.constScanLine(y);
    for (int x = 0; x < src.width(); ++x) {
      const ColorLib::Rgb rgb{line[x * 3] / 255.0, line[x * 3 + 1] / 255.0, line[x * 3 + 2] / 255.0};
      const auto hls = ColorLib::ColorMath::rgbToHls(rgb);
      const int lBin = std::clamp(static_cast<int>(std::round(hls.l * 255.0)), 0, 255);
      lHist[lBin]++;
    }
  }

  const auto lutL = makeCdfLut(lHist, total);

  QImage dst(src.size(), src.format());
  for (int y = 0; y < src.height(); ++y) {
    const auto *sLine = src.constScanLine(y);
    auto *dLine = dst.scanLine(y);
    for (int x = 0; x < src.width(); ++x) {
      const ColorLib::Rgb rgb{sLine[x * 3] / 255.0, sLine[x * 3 + 1] / 255.0, sLine[x * 3 + 2] / 255.0};
      auto hls = ColorLib::ColorMath::rgbToHls(rgb);
      const int lBin = std::clamp(static_cast<int>(std::round(hls.l * 255.0)), 0, 255);
      hls.l = lutL[lBin] / 255.0;

      const auto res = ColorLib::ColorMath::hlsToRgb(hls);
      dLine[x * 3] = static_cast<uchar>(std::round(res.r * 255.0));
      dLine[x * 3 + 1] = static_cast<uchar>(std::round(res.g * 255.0));
      dLine[x * 3 + 2] = static_cast<uchar>(std::round(res.b * 255.0));
    }
  }
  return dst;
}

QImage Ops::applyFilter(const QImage &src, int type, int radius) {
  if (type <= 0 || radius <= 0) return src;

  const int w = src.width();
  const int h = src.height();
  const int winSize = (2 * radius + 1) * (2 * radius + 1);
  const int midIdx = winSize / 2;

  QImage dst(src.size(), src.format());
  std::vector<uchar> rWin(winSize);
  std::vector<uchar> gWin(winSize);
  std::vector<uchar> bWin(winSize);

  for (int y = 0; y < h; ++y) {
    auto *dLine = dst.scanLine(y);
    for (int x = 0; x < w; ++x) {
      int idx = 0;
      for (int dy = -radius; dy <= radius; ++dy) {
        const int ny = std::clamp(y + dy, 0, h - 1);
        const auto *sLine = src.constScanLine(ny);
        for (int dx = -radius; dx <= radius; ++dx) {
          const int nx = std::clamp(x + dx, 0, w - 1);
          rWin[idx] = sLine[nx * 3];
          gWin[idx] = sLine[nx * 3 + 1];
          bWin[idx] = sLine[nx * 3 + 2];
          idx++;
        }
      }

      if (type == 1) {
        std::nth_element(rWin.begin(), rWin.begin() + midIdx, rWin.end());
        std::nth_element(gWin.begin(), gWin.begin() + midIdx, gWin.end());
        std::nth_element(bWin.begin(), bWin.begin() + midIdx, bWin.end());
        dLine[x * 3] = rWin[midIdx];
        dLine[x * 3 + 1] = gWin[midIdx];
        dLine[x * 3 + 2] = bWin[midIdx];
      } else if (type == 2) {
        dLine[x * 3] = *std::min_element(rWin.begin(), rWin.end());
        dLine[x * 3 + 1] = *std::min_element(gWin.begin(), gWin.end());
        dLine[x * 3 + 2] = *std::min_element(bWin.begin(), bWin.end());
      } else if (type == 3) {
        dLine[x * 3] = *std::max_element(rWin.begin(), rWin.end());
        dLine[x * 3 + 1] = *std::max_element(gWin.begin(), gWin.end());
        dLine[x * 3 + 2] = *std::max_element(bWin.begin(), bWin.end());
      }
    }
  }
  return dst;
}