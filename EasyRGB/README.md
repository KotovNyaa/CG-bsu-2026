# ColorConverter

Десктопное приложение для взаимного перевода и анализа цветовых моделей: **RGB**, **CMYK**, **HLS**, **HSV** и хроматической диаграммы **CIE 1931 xy (МКО)**.

## Реализованный функционал

* **Синхронный пересчёт:** моментальное обновление всех параметров при изменении любой координаты в RGB, CMYK или HLS.
* **3 способа задания цвета:**
  * Слайдеры для каждого канала.
  * Числовой ввод (спинбоксы и форматированная текстовая строка).
  * 2D-палитра (HSV) и график МКО.
* **График МКО (CIE 1931 xy):** спектральный локус, цветовой треугольник sRGB, белая точка D65 и динамический маркер координат $(x, y)$ в реальном времени.
* **Экспорт:** вывод цвета в HEX, CSS (`rgb`, `hsl`), OpenGL, Java, .NET.
* **Производительность:** ~56 МБ RAM, 0% CPU в покое, кэширование векторных градиентов.

---

## Запуск готовых файлов

### Linux
```bash
chmod +x Color_Converter-x86_64.AppImage
./Color_Converter-x86_64.AppImage
```

### Windows
Запуск файла `ColorConverter.exe` (портативный, без внешних зависимостей).

---

## Сборка из исходников

### Требования
* Компилятор C++17 (GCC, Clang, MSVC)
* CMake 3.16+
* Qt 5 или Qt 6 (`Widgets`)

### Сборка (Linux)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/ColorConverter
```

### Сборка (Windows)
```cmd
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\Release\ColorConverter.exe
```
