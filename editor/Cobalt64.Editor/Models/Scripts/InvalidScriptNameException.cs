using System;

namespace Cobalt64.Editor.Models.Scripts;

public class InvalidScriptNameException : ArgumentException
{
    public InvalidScriptNameException() 
        : base("The provided script name is invalid.") { }

    public InvalidScriptNameException(string? message) 
        : base(message) { }

    public InvalidScriptNameException(string? message, Exception? innerException) 
        : base(message, innerException) { }

    public InvalidScriptNameException(string? message, string? paramName) 
        : base(message, paramName) { }

    public InvalidScriptNameException(string? message, string? paramName, Exception? innerException) 
        : base(message, paramName, innerException) { }
}
