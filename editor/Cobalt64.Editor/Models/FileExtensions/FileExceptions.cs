/*
 * Copyright (c) 2026 Shadee
 * SPDX-License-Identifier: BSD-3-Clause
 */
using System;
using System.IO;

namespace Cobalt64.Editor.Models.FileExtensions;

public class PathException : IOException
{
    public string? InvalidPath { get; }

    public PathException() 
        : base("The provided path is invalid.") { }

    public PathException(string message) 
        : base(message) { }

    public PathException(string message, Exception innerException) 
        : base(message, innerException) { }

    public PathException(string message, string invalidPath) 
        : base(message)
    {
        InvalidPath = invalidPath;
    }
}

public class InvalidNameException : IOException
{
    public string? InvalidName { get; }
    public eStorageItemType StorageItemType { get; }

    public InvalidNameException(eStorageItemType storageItemType) 
        : this("The provided name is invalid.", storageItemType) { }

    public InvalidNameException(string message, eStorageItemType storageItemType) 
        : this(message, innerException: null, invalidName: null, storageItemType) { }

    public InvalidNameException(string message, Exception innerException, eStorageItemType storageItemType) 
        : this(message, innerException, invalidName: null, storageItemType) { }

    public InvalidNameException(string message, string invalidName, eStorageItemType storageItemType) 
        : this(message, innerException: null, invalidName, storageItemType) { }
    
    private InvalidNameException(string message, Exception? innerException, string? invalidName, eStorageItemType storageItemType) 
        : base(message, innerException)
    {
        if (!Enum.IsDefined(storageItemType))
        {
            throw new ArgumentOutOfRangeException(nameof(storageItemType), "Invalid storage item type.");
        }

        InvalidName = invalidName;
        StorageItemType = storageItemType;
    }
}

public enum eStorageItemType { Folder = 0, File }
