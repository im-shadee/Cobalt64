using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace Cobalt64.Editor.Models.Templates;

public static class TemplateManager
{
    public static string RenderTemplate(string templateFileName, Dictionary<string, string> tokens)
    {
        Assembly asm = Assembly.GetExecutingAssembly();

        string fullResourceName = asm.GetManifestResourceNames()
                .FirstOrDefault(name => name.EndsWith($".{templateFileName}", StringComparison.OrdinalIgnoreCase))
            ?? throw new FileNotFoundException($"Embedded template '{templateFileName}' not found.");

        // Get a stream to the specified resource if it exists
        using Stream stream = asm.GetManifestResourceStream(fullResourceName)!;
        using StreamReader reader = new(stream);
        string content = reader.ReadToEnd(); // Extract the entire contents of the resource
        
        // Replace each token in the file with its matching value in the dictionary
        foreach (KeyValuePair<string, string> token in tokens)
        {
            content = content.Replace($"{{{{{token.Key}}}}}", token.Value); // Replaces '{{token}}' with 'value'
        }
        
        return content;
    }
}
