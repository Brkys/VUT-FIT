/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3) ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret* definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;
  Mat4 const*const v = shader_interpretUniformAsMat4(gpu_getUniformsHandle(gpu), getUniformLocation(gpu, "viewMatrix"));
  Mat4 const*const p = shader_interpretUniformAsMat4(gpu_getUniformsHandle(gpu), getUniformLocation(gpu, "projectionMatrix"));
  Vec3 const*const pos = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  Mat4 idk;
  Vec4 wad;
  multiply_Mat4_Mat4(&idk, p, v);
  copy_Vec3Float_To_Vec4(&wad, pos, 1.f);
  multiply_Mat4_Vec4(&output->gl_Position, &idk, &wad);
  //Vec3 *const pout = vs_interpretOutputVertexAttributeAsVec3(gpu, output,0);
  //Vec3 *const nout = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);
  copy_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output,0), pos);
  copy_Vec3(vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1), vs_interpretInputVertexAttributeAsVec3(gpu,input, 1));
}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  (void)output;
  (void)input;
  (void)gpu;
  Vec3 const*norm = fs_interpretInputAttributeAsVec3(gpu, input, 1);
  Vec3 const*pos = fs_interpretInputAttributeAsVec3(gpu, input, 0);
  Vec3 const*const cam = shader_interpretUniformAsVec3(gpu_getUniformsHandle(gpu), getUniformLocation(gpu, "cameraPosition"));
  Vec3 const*const lig = shader_interpretUniformAsVec3(gpu_getUniformsHandle(gpu), getUniformLocation(gpu, "lightPosition"));
  //Deklarace vektorů a odstínu (nemám nejmenší tušení, jak jsem měl pojmenovat proměnné)
  Vec3 res, myvec, myvec2, myvec3, myvec4, yas, mirs, bar, mat2, mat3, mat5;
  float const bri = 40.f;
  double mir = 0.0;
  //Práce s funkcemi
  sub_Vec3(&res, lig, pos);
  //Normalizace světla
  normalize_Vec3(&myvec, &res);
  normalize_Vec3(&myvec2, norm);
  init_Vec3(&myvec3, 0, 1, 0);
  init_Vec3(&myvec4, 1, 1, 1);
  float brightness = fmaxf(dot_Vec3(&myvec, &myvec2), 0.0);
  if (brightness > 0.0)
  {
    sub_Vec3(&mat2, pos, cam);
    normalize_Vec3(&mat3, &mat2);
    reflect(&mat5, &myvec, &myvec2);
    float myf = fmaxf(dot_Vec3(&mat5, &mat3), 0.0);
    mir = pow(myf, bri);
  }
  multiply_Vec3_Float(&yas, &myvec3, brightness);
  multiply_Vec3_Float(&mirs, &myvec4, (float)mir);
  add_Vec3(&bar, &yas, &mirs);
  copy_Vec3Float_To_Vec4(&output->color, &bar, 1.f);
}

/// @}
