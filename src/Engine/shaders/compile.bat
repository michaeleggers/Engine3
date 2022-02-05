@echo off

%VULKAN_SDK%/bin/glslc animatedModel.vert -o ../../../bin/data/shaders/animatedModel_vert.spv
%VULKAN_SDK%/bin/glslc animatedModel.frag -o ../../../bin/data/shaders/animatedModel_frag.spv

