#include "glsl_compiler.h"

namespace vkb
{
    namespace
    {
        inline EShLanguage FindShaderLanguage(vk::ShaderStageFlagBits stage)
        {
            switch (stage)
            {
            case vk::ShaderStageFlagBits::eVertex:
                return EShLangVertex;

            case vk::ShaderStageFlagBits::eTessellationControl:
                return EShLangTessControl;

            case vk::ShaderStageFlagBits::eTessellationEvaluation:
                return EShLangTessEvaluation;

            case vk::ShaderStageFlagBits::eGeometry:
                return EShLangGeometry;

            case vk::ShaderStageFlagBits::eFragment:
                return EShLangFragment;

            case vk::ShaderStageFlagBits::eCompute:
                return EShLangCompute;

            case vk::ShaderStageFlagBits::eRaygenKHR:
                return EShLangRayGen;

            case vk::ShaderStageFlagBits::eAnyHitKHR:
                return EShLangAnyHit;

            case vk::ShaderStageFlagBits::eClosestHitKHR:
                return EShLangClosestHit;

            case vk::ShaderStageFlagBits::eMissKHR:
                return EShLangMiss;

            case vk::ShaderStageFlagBits::eIntersectionKHR:
                return EShLangIntersect;

            case vk::ShaderStageFlagBits::eCallableKHR:
                return EShLangCallable;

            case vk::ShaderStageFlagBits::eMeshEXT:
                return EShLangMesh;

            case vk::ShaderStageFlagBits::eTaskEXT:
                return EShLangTask;

            default:
                return EShLangVertex;
            }
        }
    }

    glslang::EShTargetLanguage GLSLCompiler::env_target_language                = glslang::EShTargetLanguage::EShTargetNone;
    glslang::EShTargetLanguageVersion GLSLCompiler::env_target_language_version = static_cast<glslang::EShTargetLanguageVersion>(0);

    void GLSLCompiler::set_target_environment(glslang::EShTargetLanguage target_language, glslang::EShTargetLanguageVersion target_language_version)
    {
        GLSLCompiler::env_target_language         = target_language;
        GLSLCompiler::env_target_language_version = target_language_version;
    }

    void GLSLCompiler::reset_target_environment()
    {
        GLSLCompiler::env_target_language         = glslang::EShTargetLanguage::EShTargetNone;
        GLSLCompiler::env_target_language_version = static_cast<glslang::EShTargetLanguageVersion>(0);
    }

    bool GLSLCompiler::compile_to_spirv(vk::ShaderStageFlagBits       stage,
                                        const std::vector<uint8_t>&   glsl_source,
                                        const std::string&            entry_point,
                                        const core::HPPShaderVariant& shader_variant,
                                        std::vector<std::uint32_t>&   spirv)
    {
        // Initialize glslang library
        glslang::InitializeProcess();

        EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

        EShLanguage language = FindShaderLanguage(stage);
        std::string source = std::string(glsl_source.begin(), glsl_source.end());

        const char* file_name_list[1] = { "" };
        const char* shader_source = reinterpret_cast<const char*>(source.data());

        glslang::TShader shader(language);
        shader.setStringsWithLengthsAndNames(&shader_source, nullptr, file_name_list, 1);
        shader.setEntryPoint(entry_point.c_str());
        shader.setSourceEntryPoint(entry_point.c_str());
        shader.setPreamble(shader_variant.get_preamble().c_str());
        shader.addProcesses(shader_variant.get_processes());
        if (GLSLCompiler::env_target_language != glslang::EShTargetLanguage::EShTargetNone)
        {
            shader.setEnvTarget(GLSLCompiler::env_target_language, GLSLCompiler::env_target_language_version);
        }

        // Add shader to new program object
        glslang::TProgram program;
        program.addShader(&shader);

        // Link program
        if (!program.link(messages))
        {
            return false;
        }

        glslang::TIntermediate* intermediate = program.getIntermediate(language);

        // Translate to SPIRV
        if (!intermediate)
        {
            return false;
        }

        glslang::GlslangToSpv(*intermediate, spirv);

        // Shutdown glslang library
        glslang::FinalizeProcess();

        return true;
    }
}