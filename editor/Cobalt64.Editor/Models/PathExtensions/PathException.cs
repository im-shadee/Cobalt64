using System;
using System.IO;

namespace Cobalt64.Editor.Models.PathExtensions;

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
