/*
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
using System.IO;

namespace Cobalt64.Editor.Models.FileExtensions;

public static class FileCreator
{
    private const string m_kDefaultFileExtension = ".txt";
    
    /// <summary>
    /// Creates a folder at <paramref name="path"/> with name <paramref name="folderName"/>.
    /// </summary>
    /// <returns>An object that represents the created directory.</returns>
    public static DirectoryInfo CreateFolder(string path, string relativeFolderPath)
    {
        if (!FileValidator.ValidatePath(path))
        {
            throw new PathException($"Directory '{path}' does not exist or contains illegal characters.", path);
        }

        if (!FileValidator.ValidateRelativePath(relativeFolderPath))
        {
            throw new InvalidNameException(
                $"Folder path '{relativeFolderPath}' is invalid or contains illegal characters ({FileValidator.FormattedFileChars}).", 
                relativeFolderPath, 
                eStorageItemType.Folder
            );
        }
        
        string fullPath = Path.Join(path, relativeFolderPath);
        return Directory.CreateDirectory(fullPath);
    }

    /// <summary>
    /// Creates a file at <paramref name="path"/> with name <paramref name="fileName"/>.
    /// </summary>
    /// <returns>An instance of <see cref="StreamWriter"/> on the created file.</returns>
    /// <remarks>Assumes that <paramref name="fileName"/> contains an extension. Otherwise, it will append .txt as the default extension.</remarks>
    public static StreamWriter CreateFile(string path, string fileName)
    {
        if (!FileValidator.ValidatePath(path))
        {
            throw new PathException($"Directory '{path}' does not exist or contains illegal characters.", path);
        }
        
        if (!FileValidator.ValidateName(fileName))
        {
            throw new InvalidNameException(
                $"File name cannot contain any invalid characters ({FileValidator.FormattedFileChars}).", 
                fileName, 
                eStorageItemType.File
            );
        }
        
        if (!Path.HasExtension(fileName)) fileName += m_kDefaultFileExtension;
        string fullPath = Path.Join(path, fileName);
        return File.CreateText(fullPath);
    }
}
