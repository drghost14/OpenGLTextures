## OpenGLTextures

OpenGLTextures is a minimal C++ starter template demonstrating real-time textured quad rendering with Raylib, GLAD, and modern OpenGL. It provides modular classes for shader management, texture loading, VAO setup, and buffer handling—perfect for building interactive demos, ASMR-style visuals, or extending into full-blown graphics engines.

---

## ⚙️ Technology Stack

- Raylib — window creation, input handling, and image loading  
- GLAD — cross-platform OpenGL function loader  
- OpenGL 3.3+ — modern programmable pipeline  
- CMake — project generation for MinGW, MSVC, or Unix Makefiles  
- Makefile — simple GNU Make build logic  

---

## 📁 Project Structure

```text
OpenGLTextures/
├── data/                
│   └── car.png          # Sample texture image  
├── shaders/             
│   ├── vertex.glsl      # Vertex shader source  
│   └── fragment.glsl    # Fragment shader source  
├── src/                 
│   └── main.cpp         # Entry point and demo code  
├── CMakeLists.txt       # CMake build configuration  
├── Makefile             # GNU Make build rules  
├── README.md            # This file  
└── LICENSE              # MIT license  
```

---

## 🚀 Quick Start

1. Clone the repository  
   ```bash
   git clone https://github.com/drghost14/OpenGLTextures.git
   cd OpenGLTextures
   ```
2. Install dependencies  
   - Raylib development files  
   - GLAD headers and source  
   - CMake (or GNU Make)  
3. Build the project  
   - With CMake  
     ```bash
     mkdir build && cd build
     cmake ..
     cmake --build .
     ```  
   - Or with Make  
     ```bash
     make
     ```
4. Run the demo  
   ```bash
   ./OpenGLTextures
   ```
   A window will open displaying a textured quad on a light background.

---

## 🧩 Core Classes

- **ShaderClass**  
  Loads, compiles, links, and manages GLSL shaders.  
- **TextureClass**  
  Loads images via Raylib, uploads to GPU, sets filtering modes, and generates mipmaps.  
- **VaoClass**  
  Creates and manages a vertex array object for attribute binding.  
- **BufferClass**  
  Wraps VBO/EBO creation and data buffering with a simple API.  

---

## 🔧 Customization Ideas

- Swap in different images at runtime for an ASMR-style reveal  
- Add keyboard controls to toggle between linear and nearest filtering  
- Extend ShaderClass with uniform setters for floats, vectors, and matrices  
- Layer multiple quads with blending for soft visual transitions  

---

## 📄 License

### 🔐 License

Rayminder is released under the [MIT License](LICENSE) —  
you’re free to use, distribute, and remix this application for personal or educational purposes.  

> Attribution is appreciated. Raylib and other dependencies maintain their own license terms.

---
