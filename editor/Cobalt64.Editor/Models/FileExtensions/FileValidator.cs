/*
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
using System;
using System.Buffers;
using System.IO;

namespace Cobalt64.Editor.Models.FileExtensions;

public static class FileValidator
{
    // Invalid chars
    public static readonly char[] InvalidFileChars = Path.GetInvalidFileNameChars();
    public static readonly char[] InvalidPathChars = Path.GetInvalidPathChars();
    public static readonly string FormattedFileChars = FormatInvalidChars(InvalidFileChars);
    
    private static readonly SearchValues<string> s_ReservedWindowsNames = SearchValues.Create(
        [
            "CON", "PRN", "AUX", "NUL",
            "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
            "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
        ],
        StringComparison.OrdinalIgnoreCase);

    public static bool ValidatePath(string path)
    {
        return !string.IsNullOrWhiteSpace(path)
               && !path.ContainsAny(InvalidPathChars)
               && Directory.Exists(path);
    }

    public static bool ValidateName(string? name)
    {
        if (string.IsNullOrWhiteSpace(name)) return false;

        // Reject '.' and '..' (path traversal) as well as trailing spaces/dots
        if (name is "." or ".." || name.EndsWith(' ') || name.EndsWith('.')) return false;
        
        // Reject illegal filename characters
        if (name.ContainsAny(InvalidFileChars)) return false; 

        // Reject Windows reserved device names (e.g., "CON" or "CON.txt")
        if (OperatingSystem.IsWindows())
        {
            string nameWithoutExt = Path.GetFileNameWithoutExtension(name);
            if (s_ReservedWindowsNames.Contains(nameWithoutExt)) return false;
        }

        return true;
    }
    
    public static bool ValidateRelativePath(string? relativePath)
    {
        if (string.IsNullOrWhiteSpace(relativePath)) return false;

        // Reject rooted paths like "/Assets" or "C:\Assets"
        if (Path.IsPathRooted(relativePath)) return false;

        // Split on both '/' and '\' and validate each folder name individually
        string[] segments = relativePath.Split(['/', '\\'], StringSplitOptions.RemoveEmptyEntries);

        if (segments.Length == 0) return false;

        foreach (string segment in segments)
        {
            if (!ValidateName(segment)) return false;
        }
        return true;
    }
    
    internal static string FormatInvalidChars(char[] invalidChars)
    {
        char[] visible = Array.FindAll(invalidChars, c => !char.IsControl(c));
        return visible.Length > 0 ? $"({string.Join(", ", visible)})" : string.Empty;
    }
}
