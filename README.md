# CGaV
This project started as a project for my Coputer Graphics and Visualization Classes (hence the name) but I enjoyed it so much that I started to work on diffrent things and maybe someday this will be a full fledged mini game engine

# Current features:
 - 3D Rendering using specular workflow (blinn-phong lighting, material/diffuse + specular + normal map handling, loading and drawing complex models)
 - 3D Rendering using metallic workflow (cook-torrance brdf, material/albedo+roughness+metallic+normal+ao map handling, loading and drawing complex models)
 - skyboxes (cubemaps)
 - basic post processing (3x3 kernel)
 - couple of glsl shaders for diffrent workflows
 - movable camera
 - drunk effect using postprocessing and camera movement

# Planned features:
  - deferred shading for both specular and metalic workflows
  - image based lighting (specular ibl, epic version)
  - SSAO 
  - AABB phsics
  - combine rendering and physics to create Game Objects
  - some example scene
