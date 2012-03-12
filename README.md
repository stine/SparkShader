# GPU-based particle shader

This is a very simple toy program that I hacked together in an evening to demonstrate a completely GPU-based particle engine.  The particles are not generated or processed on the CPU at all - all processing is performed in OpenGL shaders in graphics hardware.

Every frame, two shaders are executed to create the effect: one shader generates particles randomly on the surface of an arbitrary application-provided mesh, and following that, another shader processes each particle in the scene, "aging" it over time.  Each particle has an associated world space coordinate, world space velocity, and age, which amounts to 32 bytes of data.

The simple calculations used in this shader are extremely fast in the GPU.  In the provided video, I only tested with ~100,000 particles due to OpenGL geometry shader limits to the number of particles that can be generated from a single input primitive (triangle), and because of my laziness in not creating a higher resolution input mesh.  I imagine that this could easily scale to millions of particles per frame.

If I get around to it, I'd like to add some sort of turbulance, maybe an initial velocity to particles being birthed from the mesh surface, and the use of Tesselation shaders to generate far more than just 80 particles per triangle per frame.

<iframe src="http://player.vimeo.com/video/38206376?title=0&amp;byline=0&amp;portrait=0" width="400" height="300" frameborder="0" webkitAllowFullScreen mozallowfullscreen allowFullScreen></iframe><p><a href="http://vimeo.com/38206376">Simple GPU-based particle engine</a> from <a href="http://vimeo.com/user10623366">Matthew Stine</a> on <a href="http://vimeo.com">Vimeo</a>.</p>
