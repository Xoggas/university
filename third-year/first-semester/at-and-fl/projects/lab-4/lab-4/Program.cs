using System.Text;
using lab_4;

var testCases = new[]
{
    "a[a=(-(a*(a+(a*-(a)*a))));a=a;][a[a=a;];];",
    "a=a+-(a)+a;",
    "++++",
    "a-=[]()",
    "-a=[()-*]"
};

// foreach (var createParser in new Func<string, IParser>[] { x => new Parser(x), x => new Mpr(x) })
// {
//     foreach (var testCase in testCases)
//     {
//         var parser = createParser(testCase);
//
//         Console.WriteLine(parser.GetType().Name);
//
//         try
//         {
//             var appliedRules = parser.Parse();
//
//             Console.WriteLine($"Successfully parsed \"{testCase}\"");
//
//             Console.WriteLine($"Applied rules: {string.Join(", ", appliedRules)}");
//
//             Console.WriteLine($"Sorted applied rules: {string.Join(", ", appliedRules.Distinct().OrderBy(x => x))}\n");
//         }
//         catch (Exception e)
//         {
//             Console.WriteLine($"Failed to parse \"{testCase}\"");
//
//             Console.WriteLine($"Error: {e.Message}\n");
//         }
//     }
// }

string ReplaceFirst(string original, string oldValue, string newValue)
{
    var index = original.IndexOf(oldValue, StringComparison.Ordinal);

    if (index < 0)
    {
        return original;
    }

    return new StringBuilder(original)
        .Remove(index, oldValue.Length)
        .Insert(index, newValue)
        .ToString();
}

var output = "S";
var steps = new[]
{
    1, 5, 10, 17, 11, 14, 10, 17, 12, 17, 11, 15, 17, 12, 3
};

var rules = new Dictionary<int, (string, string)>
{
    { 1, ("S", "aN2;N1") },
    { 2, ("N1", "aN2;N1") },
    { 3, ("N1", "") },
    { 4, ("N2", "[S]N3") },
    { 5, ("N2", "=E") },
    { 6, ("N3", "[S]") },
    { 7, ("N3", "") },
    { 8, ("E", "(E)T'E'") },
    { 9, ("E", "-(E)T'E'") },
    { 10, ("E", "aT'E'") },
    { 11, ("E'", "+TE'") },
    { 12, ("E'", "") },
    { 13, ("T", "(E)T'") },
    { 14, ("T", "-(E)T'") },
    { 15, ("T", "aT'") },
    { 16, ("T'", "*PT'") },
    { 17, ("T'", "") },
    { 18, ("P", "(E)") },
    { 19, ("P", "-(E)") },
    { 20, ("P", "a") }
};

var sb = new StringBuilder();
sb.AppendLine("digraph LeftDerivation {");
sb.AppendLine("\trankdir=\"UD\";");
sb.AppendLine("\tnode [shape=none];");
sb.AppendLine("\tedge [arrowhead=vee];");

foreach (var (ruleIndex, i) in steps.Select((x, i) => (x, i + 1)))
{
    if (i == 1)
    {
        sb.AppendLine($"\tS -> Step1 [label=\"{ruleIndex}\"];");

        output = ReplaceFirst(output, rules[ruleIndex].Item1, rules[ruleIndex].Item2);

        sb.AppendLine($"\tStep1 [label=\"{output}\"];");
    }
    else
    {
        sb.AppendLine($"\tStep{i - 1} -> Step{i} [label=\"{ruleIndex}\"];");

        output = ReplaceFirst(output, rules[ruleIndex].Item1, rules[ruleIndex].Item2);

        sb.AppendLine($"\tStep{i} [label=\"{output}\"];");
    }

    sb.AppendLine();
}

sb.AppendLine("}");

Console.WriteLine(sb);