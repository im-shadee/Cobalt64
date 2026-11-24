using System;
using System.IO;

namespace Cobalt64.Editor.Models.PathExtensions;

public static class PathValidator
{
    // Invalid chars
    public static readonly char[] InvalidFileChars = Path.GetInvalidFileNameChars();
    public static readonly char[] InvalidPathChars = Path.GetInvalidPathChars();
    public static readonly string FormattedFileChars = FormatInvalidChars(InvalidFileChars);

    public static bool ValidatePath(string path)
    {
        return Path.Exists(path)
               && !path.ContainsAny(InvalidFileChars);
    }
    
    internal static string FormatInvalidChars(char[] invalidChars)
    {
        char[] visible = Array.FindAll(invalidChars, c => !char.IsControl(c));
        return visible.Length > 0 ? $"({string.Join(", ", visible)})" : string.Empty;
    }
}
