using System.Text;
using lab_4;

public class Mpr : IParser
{
    private const char Eof = '$';
    private const string EndOfStack = "#";

    private readonly string _input;
    private readonly Stack<string> _stack = [];
    private readonly List<int> _appliedRules = [];
    private int _inputPtr;

    private string _output = "S";

    private readonly Dictionary<int, (string, string)> _rules = new()
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

    public Mpr(string input)
    {
        _input = input;
        _stack.Push(EndOfStack);
        _stack.Push("S");
    }

    private string StackTop => _stack.Peek();
    private char CurrentToken => _inputPtr >= _input.Length ? Eof : _input[_inputPtr];

    public List<int> Parse()
    {
        var isDone = false;

        while (isDone is false)
        {
            var currentToken = CurrentToken.ToString();

            switch (currentToken, StackTop)
            {
                case (";", ";"):
                case ("]", "]"):
                case ("(", "("):
                case (")", ")"):
                    PopAndAdvance();
                    continue;
            }

            switch (StackTop)
            {
                case "S":
                    S();
                    break;
                case "N1":
                    N1();
                    break;
                case "N2":
                    N2();
                    break;
                case "N3":
                    N3();
                    break;
                case "E":
                    E();
                    break;
                case "E'":
                    E_Mark();
                    break;
                case "T":
                    T();
                    break;
                case "T'":
                    T_Mark();
                    break;
                case "P":
                    P();
                    break;
                default:
                    isDone = true;
                    break;
            }
        }

        if (StackTop != EndOfStack || CurrentToken != Eof)
        {
            throw ThrowUnexpectedToken();
        }

        return _appliedRules;
    }

    private void AppliedRule(int ruleNumber)
    {
        _appliedRules.Add(ruleNumber);
        _output = ReplaceFirst(_output, _rules[ruleNumber].Item1, _rules[ruleNumber].Item2);
        Console.WriteLine($"Intermediate output: {_output}");
    }

    private void S()
    {
        switch (CurrentToken)
        {
            case 'a':
                AppliedRule(1);
                ReplaceAndAdvance("N1", ";", "N2");
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void N1()
    {
        switch (CurrentToken)
        {
            case 'a':
                AppliedRule(2);
                ReplaceAndAdvance("N1", ";", "N2");
                break;
            case ']' or Eof:
                AppliedRule(3);
                PopAndKeep();
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void N2()
    {
        switch (CurrentToken)
        {
            case '[':
                AppliedRule(4);
                ReplaceAndAdvance("N3", "]", "S");
                break;
            case '=':
                AppliedRule(5);
                ReplaceAndAdvance("E");
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void N3()
    {
        switch (CurrentToken)
        {
            case '[':
                AppliedRule(6);
                ReplaceAndAdvance("]", "S");
                break;
            case ';':
                AppliedRule(7);
                PopAndKeep();
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void E()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(8);
                ReplaceAndAdvance("E'", "T'", ")", "E");
                break;
            case '-':
                AppliedRule(9);
                ReplaceAndAdvance("E'", "T'", ")", "E", "(");
                break;
            case 'a':
                AppliedRule(10);
                ReplaceAndAdvance("E'", "T'");
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void E_Mark()
    {
        switch (CurrentToken)
        {
            case '+':
                AppliedRule(11);
                ReplaceAndAdvance("E'", "T");
                break;
            case Eof or ')' or ';':
                AppliedRule(12);
                PopAndKeep();
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void T()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(13);
                ReplaceAndAdvance("T'", ")", "E");
                break;
            case '-':
                AppliedRule(14);
                ReplaceAndAdvance("T'", ")", "E", "(");
                break;
            case 'a':
                AppliedRule(15);
                ReplaceAndAdvance("T'");
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void T_Mark()
    {
        switch (CurrentToken)
        {
            case '*':
                AppliedRule(16);
                ReplaceAndAdvance("T'", "P");
                break;
            case Eof or '+' or ')' or ';':
                AppliedRule(17);
                PopAndKeep();
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private void P()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(18);
                ReplaceAndAdvance(")", "E");
                break;
            case '-':
                AppliedRule(19);
                ReplaceAndAdvance(")", "E", "(");
                break;
            case 'a':
                AppliedRule(20);
                PopAndAdvance();
                break;
            default:
                throw ThrowUnexpectedToken();
        }
    }

    private Exception ThrowUnexpectedToken()
    {
        return new Exception($"Unexpected token {CurrentToken} at position {_inputPtr}");
    }

    private void Push(string symbol)
    {
        _stack.Push(symbol);
    }

    private void PopAndKeep()
    {
        _stack.Pop();
    }

    private void PopAndAdvance()
    {
        PopAndKeep();
        Advance();
    }

    private void ReplaceAndAdvance(params string[] symbols)
    {
        PopAndKeep();

        foreach (var symbol in symbols)
        {
            Push(symbol);
        }

        Advance();
    }

    private void Advance()
    {
        _inputPtr++;
    }

    public static string ReplaceFirst(string original, string oldValue, string newValue)
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
}