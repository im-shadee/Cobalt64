using System.Collections.Generic;
using System.IO;
using Cobalt64.Editor.Models.Utils;

namespace Cobalt64.Editor.Models.Templates;

public static class ActorTemplateWriter
{
    public static void CreateTemplate(string targetFolder, string behaviorName)
    {
        string fileName = behaviorName.ToSnakeCase(); // Sanitize input to ensure C code contains valid names
        
        // Create a dictionary to map tokens in the template file with the names chosen for the behavior script
        Dictionary<string, string> tokens = new Dictionary<string, string>
        {
            ["FILE_NAME"] = fileName,
            ["PREFIX"] = fileName
        };
        
        // Render source/header templates
        string headerCode = TemplateManager.RenderTemplate("actor_behavior.h.template", tokens);
        string sourceCode = TemplateManager.RenderTemplate("actor_behavior.c.template", tokens);
        
        // Write the generated files to the specified folder
        File.WriteAllText(Path.Join(targetFolder, $"{fileName}.h"), headerCode);
        File.WriteAllText(Path.Join(targetFolder, $"{fileName}.c"), sourceCode);
    }
}
