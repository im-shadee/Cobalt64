using System.Collections.Generic;
using System.IO;
using System.Linq;
using Cobalt64.Editor.Models.FileExtensions;
using Cobalt64.Editor.ViewModels;

namespace Cobalt64.Editor.Models.Builds;

public static class ProjectFileGenerator
{
    public static void GenerateProjectFiles(string destinationFolder, string projectFolder)
    {
        WriteConfigFile(destinationFolder, null); // TODO: Map settings
        WriteActorReg(destinationFolder, Path.Join(projectFolder, "Scripts/"));
    }
    
    internal static void WriteConfigFile(string targetDirectory, ProjectSettingsViewModel settings)
    {
        string safeGameName = settings.GameName.Replace("\"", "\\\"");

        string content = $$"""
                 /* AUTO-GENERATED AT BUILD TIME. DO NOT MODIFY. */
                 #include <cobalt64/config_data.h>
                 #include <stdint.h>

                 const CobaltEngineConfig g_cobalt_conf = {
                     .res_width = {{settings.Resolution.x}},
                     .res_height = {{settings.Resolution.y}},
                     .framebuffer_count = {{settings.FramebufferCount}},
                     .dither_mode = {{settings.DitherMode}},
                     .target_fps = {{settings.TargetFramerate}},
                     .default_scene_index = {{settings.DefaultSceneIndex}},
                     .target_mem_bytes = {{settings.TargetMemoryUsage}},
                     .scene_data_mem = {{settings.SceneDataMemory}},
                     .global_data_mem = {{settings.GlobalDataMemory}},
                     .game_name = "{{safeGameName}}",
                     .scene_data_path = "./{{safeGameName}}_data/scenes.bin",
                 };
                 """;

        using StreamWriter sw = FileCreator.CreateFile(targetDirectory, "cobalt_config.c");
        sw.Write(content);
    }

    internal static void WriteActorReg(string targetDirectory, string scriptsFolder)
    {
        List<string> actorBehaviors = new List<string>();
        List<string> triggerBehaviors = new List<string>();
        
        // TODO: Inspect each file in /Scripts/ to find lifecycle/trigger functions
        // TODO: Write each function into its dedicated table
        
        // Format array entries, 4 per line

        // Write the actual file contents
        string content = $$"""
                 /* AUTO-GENERATED AT BUILD TIME. DO NOT MODIFY. */
                 // Compiled into .rodata (ROM)
                 #include <cobalt64.h>
                 #include <stdint.h>
                 
                 /* Discovered user functions */
                 (TODO)

                 // Lookup table ordered by actor behavior ID, assigned by the editor
                 const ActorBehavior g_actor_behavior_reg[] = {
                 (TODO)
                 };
                 
                 // Lookup table ordered by trigger behavior ID, assigned by the editor
                 const TriggerActorBehavior g_trigger_behavior_reg[] = {
                 (TODO)
                 };
                 
                 const uint16_t G_ACTOR_BEHAVIOR_COUNT = {{actorBehaviors.Count}};
                 const uint16_t G_TRIGGER_BEHAVIOR_COUNT = {{triggerBehaviors.Count}};
                 """;
    }

}
