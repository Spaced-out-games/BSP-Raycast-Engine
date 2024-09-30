# Inspiration
After scouraging the web for interesting programming projects, I came across Teardown, a video game with its own custom engine. the voxels are not drawn using pixel information, but rather walked through as volumetric information in a pixel shader using raymarching.

I've also been a big fan of old engines, like those found in Quake and its derivatives (eg, Source). My goal with this engine is to create a toolset for a VR title I hope to make, but it will require a custom game engine to function.

# BSP raycaster
After learning about BSPs, I've learned that they are particularly effective at describing convex volumes, are great at reducing overdraw, and are great for visibility determination. BSPs have long been deprecated in favor of newer data structures that are suitable for dynamic change. But I think people are just not thinking about it all from the right angle. Most of time in most games, walls aren't moving or being destroyed, and even for games that have those systems, there are workarounds. Portal is an example of the kind of workarounds that can be made. Typically, BSPs are good for fast collision with large landscapes, but Portal effectively masks the BSP of the level geometry to allow players to, from an engine standpoint, walk through walls and see behind them. If you combine this with a sparse voxel octree, why couldn't you get destructible environments by making the pixel shader mask against the octree? Why couldnt you mask collision over the octree? Wouldnt you then have a destructable BSP environment? Would that not spit in the face the assumptions that have been made about BSPs?

Further, since we are using a raycaster, why couldn't game props be stored as miniature BSPs? Their collision hulls would be stored in worldspace, and during rendering, any intersection with a bounding box would transform the ray into local space. This allows us to have moving BSP structures without the need to rebuild trees.

# Efficiency
Considering that BSPs were used for some of the earliest 3D games, namely DOOM and Quake, these data structures are remarkably fast to traverse and practically eliminate overdraw. My new approach perfects the strengths of the structure while also cutting out many of the limitations.
## Less memory usage
By using raycasting, there is no need to store a buffer containing the triangles of the scene, nor any need to do complicated transformations. BSPs define volume, not geometry, best use it accordingly.

## ZERO transformations
Considering we are working with rays instead of triangles, we cut down the need for transforms dramatically. Given a camera, we cast rays from the near plane to the far plane, which requires zero matrix transformations. Every ray corresponds to a pixel in screen space, and each ray has a relative translation from the center ray. The center ray is derived by getting the forward vector of the camera. The near and far multiplicative vectors (for defining start and end positions of the rays in world space) can be derived by taking the ray from the center of the screen, and adding an offset by these two vectors, multiplied by their distance from the center of the screen

## ZERO overdraw
The rays will only hit the first thing they come across, so overdraw will not be an issue

## Implicit frustum culling
Since these rays are clipped to the near and far planes, they will never render anything outside the view frustum as the rays approximate the volume of the view frustum

## Implicit backface culling
Since we will never hit a back face, backface culling is a natural byproduct of this approach

## Implicit occlusion culling
Whatever is hit first is drawn, whatever is behind it is not. However, game props are not implicitly culled. However, it does need to be noted that you could project the bounding box into screen space and test versus the depth buffer

## Implicit mipmapping
Since things that are further away are smaller, rays that hit distant objects will sample fewer texels, effectively yielding dynamic mipmapping. This is especially useful for more complicated shaders, like parallax occlusion mapping, as they are more efficient when viewed at extreme angles or from far away

I do want to note that this is untested as of now, and it's a current work in progress. Right now I'm trying to get BSPs to work before porting them over to the GPU, where raycasting will be implemented in a shader.
