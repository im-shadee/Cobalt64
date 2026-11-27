/*
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Cobalt64.Editor.Models.FileExtensions;

namespace Cobalt64.Editor.Views;

public partial class MainWindow : Window
{
    private static readonly string[] s_ProjectFolders =
    {
        "Assets/Models", "Assets/Textures", "Assets/Scenes", // Asset folders (with models, textures, scene data)
        ".cobalt/generated", ".cobalt/dfs", ".cobalt/obj", // Cobalt-specific folders
        "Scripts", "ProjectSettings", // User/Project settings and scripts
    };
    
    public MainWindow()
    {
        InitializeComponent();
    }

    internal async void OnCreateNewProjectClicked(object sender, RoutedEventArgs e)
    {
        try
        {
            IReadOnlySet<string> selectedFolders = await FilePicker.OpenFolder(this);
            string? projectPath = selectedFolders.FirstOrDefault();
            
            if (string.IsNullOrWhiteSpace(projectPath)) return;

            // Create project hierarchy:
            // ProjectName
            // |    Assets
            // |    |   Models
            // |    |   Textures
            // |    |   Scenes
            // |    Scripts
            // |    ProjectSettings
            Console.WriteLine($"Creating project at {projectPath}...");
            DirectoryInfo currentDirectory;

            foreach (string relativePath in s_ProjectFolders)
            {
                currentDirectory = FileCreator.CreateFolder(projectPath, relativePath);
                Console.WriteLine($"Created folder {currentDirectory.FullName}.");
            }
            
            LaunchEditorWindow();
        }
        catch (Exception ex)
        {
            // TODO: Open error window with ex.Message
        }
    }
    
    internal void OnOpenProjectClicked(object sender, RoutedEventArgs e)
    {
        Console.WriteLine("Open project");
        LaunchEditorWindow();
    }

    internal void LaunchEditorWindow()
    {
        // TODO: Open the editor window
        Close(); // Close the selection window
    }
}