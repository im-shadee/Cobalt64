using System;
using System.Text.RegularExpressions;

namespace Cobalt64.Editor.Models.Utils;

public static partial class StringExtensions
{
    [GeneratedRegex(@"([a-z0-9])([A-Z])")]
    private static partial Regex CamelCaseRegex();

    [GeneratedRegex(@"[^a-zA-Z0-9]+")]
    private static partial Regex NonAlphanumericRegex();
    
    public static string ToSnakeCase(this string str)
    {
        // Remove trailing whitespaces and replace pattern aaaAAA with aaa_AAA
        string s = CamelCaseRegex().Replace(str.Trim(), "$1_$2");

        // Replace non-alphanumeric characters with '_' and strip any '_' at start/end then convert to lowercase
        s = NonAlphanumericRegex().Replace(s, "_").Trim('_').ToLowerInvariant();
        
        // Guard against invalid inputs (empty inputs after filtering) (e.g. "!!!" => "")
        if (string.IsNullOrEmpty(s)) throw new ArgumentException("Invalid name.", nameof(str));
        
        if (char.IsDigit(s[0])) s = "_" + s; // Prepend a '_' before the string if it starts with a digit
        return s;
    }
}
