using System;
using System.Globalization;
using System.IO;
using Cobalt64.Editor.Models.PathExtensions;

namespace Cobalt64.Editor.Models.Scripts;

public static class ScriptWriter
{
    private static readonly string[] s_ActorFuncSignatures =
    {
        "on_add(uint16_t obj_idx, ActorData actor_data)",
        "start(uint16_t obj_idx, ActorData actor_data)",
        "update(uint16_t obj_idx, ActorData actor_data, float dt)",
        "on_destroy(uint16_t obj_idx, ActorData actor_data)"
    };

    internal enum eScriptLanguage
    {
        C = 0,
        H
    };
    
    /// <summary>
    /// Writes a .c and a .h file in <paramref name="path"/> with <paramref name="fileName"/> as their name.
    /// </summary>
    public static void WriteScript(string path, string fileName) 
    {
        WriteScriptFile(path, fileName, eScriptLanguage.C);
        WriteScriptFile(path, fileName, eScriptLanguage.H);
    }

    internal static void WriteScriptFile(string path, string fileName, eScriptLanguage scLang)
    {
        // General validation
        if (!PathValidator.ValidatePath(path))
        {
            throw new PathException($"Path '{path}' contains illegal characters or is empty.", path);
        }

        if (!ValidateScriptName(fileName))
        {
            throw new InvalidScriptNameException(
                $"Script name cannot contain any invalid characters ({PathValidator.FormattedFileChars}).",
                nameof(fileName)
            );
        }

        string fullPath = Path.Join(path, fileName, GetExtensionFor(scLang));
        StreamWriter sw = File.CreateText(fullPath);
        sw.Write(GetContentFor(scLang, fileName));
    }

    internal static bool ValidateScriptName(string fileName) => fileName.ContainsAny(PathValidator.InvalidFileChars);

    internal static string GetExtensionFor(eScriptLanguage scLang)
    {
        return scLang switch
        {
            eScriptLanguage.C => ".c",
            eScriptLanguage.H => ".h",
            _ => throw new ArgumentOutOfRangeException($"No extension for language {scLang}."),
        };
    }

    internal static string GetContentFor(eScriptLanguage scLang, string fileName)
    {
        return scLang switch
        {
            eScriptLanguage.C => $"#include \"{fileName}.h\"\n",
            eScriptLanguage.H => GetHeaderFileContent(fileName),
            _ => string.Empty,
        };
    }
    
    internal static string GetHeaderFileContent(string fileName)
    {
        string structName = ToPascalCase(fileName);
        string res = "#pragma once\n" +
                     "#include \"actors.h\"\n\n" +
                     "/* Actor Data */\n\n" +
                     "typedef struct {\n" +
                     "\t/* variables */\n" +
                     "} " + structName + ";\n\n" +
                     "/* Cobalt Lifecycle */\n\n";
        
        // Add functions (auto-generated from signatures array)
        for (int i = 0; i < s_ActorFuncSignatures.Length; i++)
        {
            // Follows format:
            // void my_script_fnSignature {
            //
            // }
            //
            res += $"void {fileName}_{s_ActorFuncSignatures[i]}" + " {\n\t\n}\n";
            if (i != s_ActorFuncSignatures.Length - 1) res += "\n";
        }

        return res;
    }

    internal static string ToPascalCase(string fileName)
    {
        return string.IsNullOrWhiteSpace(fileName) 
            ? fileName 
            : CultureInfo.InvariantCulture.TextInfo.ToTitleCase(
                fileName.Replace('_', ' ').ToLowerInvariant()
            ).Replace(" ", string.Empty);
    }
}
