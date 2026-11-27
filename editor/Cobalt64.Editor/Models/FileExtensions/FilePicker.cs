/*
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Platform.Storage;

namespace Cobalt64.Editor.Models.FileExtensions;

public static class FilePicker
{
    private static readonly bool s_CaseInsensitivePaths = OperatingSystem.IsWindows() || OperatingSystem.IsMacOS();
    
    // Use Ordinal comparison between strings and ignore case on Windows/MacOS
    private static readonly StringComparer s_StringComparer = s_CaseInsensitivePaths 
        ? StringComparer.OrdinalIgnoreCase
        : StringComparer.Ordinal;

    private static readonly FolderPickerOpenOptions s_DefaultFolderOpenOptions = new FolderPickerOpenOptions()
    {
        Title = "Select a folder",
        AllowMultiple = false,
    };
    
    private static readonly FilePickerOpenOptions s_DefaultFileOpenOptions = new FilePickerOpenOptions()
    {
        Title = "Select a file",
        AllowMultiple = false,
    };
    
    /// <summary>
    /// Opens a folder selection window from a <see cref="TopLevel"/> context and gets the selected folder paths.
    /// </summary>
    /// <param name="topLevel">The context on top of which the folder selection window appears.</param>
    /// <param name="options">Optional picker options for the selection. Defaults to unique selection with a generic title.</param>
    /// <returns>A set of unique folder paths selected by the user.</returns>
    public static Task<IReadOnlySet<string>> OpenFolder(TopLevel? topLevel, FolderPickerOpenOptions? options = null)
    {
        return PickFromStorageAsync(topLevel, provider => provider.OpenFolderPickerAsync(options ?? s_DefaultFolderOpenOptions));
    }
    
    /// <summary>
    /// Opens a file selection window from a <see cref="TopLevel"/> context and gets the selected file paths.
    /// </summary>
    /// <param name="topLevel">The context on top of which the file selection window appears.</param>
    /// <param name="options">Optional picker options for the selection. Defaults to unique selection with a generic title.</param>
    /// <returns>A set of unique file paths selected by the user.</returns>
    public static Task<IReadOnlySet<string>> OpenFile(TopLevel? topLevel, FilePickerOpenOptions? options = null)
    {
        return PickFromStorageAsync(topLevel, provider => provider.OpenFilePickerAsync(options ?? s_DefaultFileOpenOptions));
    }
    
    internal static async Task<IReadOnlySet<string>> PickFromStorageAsync<T>(TopLevel? topLevel, 
        Func<IStorageProvider, Task<IReadOnlyList<T>>> pickerFunc) where T : IStorageItem
    {
        HashSet<string> paths = new HashSet<string>(s_StringComparer);
        if (topLevel == null) return paths; // Empty list

        // Open the dialogue box and retrieve the selected storage items
        IReadOnlyList<T> items = await pickerFunc(topLevel.StorageProvider);

        // Build the set of all selected paths
        foreach (T selectedItem in items)
        {
            string? folderPath = selectedItem.TryGetLocalPath();
            if (string.IsNullOrWhiteSpace(folderPath)) continue; // Remove invalid paths
            paths.Add(folderPath); // Add valid paths to the set
        }

        return paths;
    }
}