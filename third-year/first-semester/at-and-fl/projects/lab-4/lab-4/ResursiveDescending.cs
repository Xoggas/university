using lab_4;

class Parser : IParser
{
    private const char Eof = '$';

    private readonly string _input;
    private readonly List<int> _appliedRules = [];
    private int _currentSymbolIndex;

    public Parser(string input)
    {
        _input = input;
    }

    private char CurrentToken => _currentSymbolIndex >= _input.Length ? Eof : _input[_currentSymbolIndex];

    public List<int> Parse()
    {
        S();
        
        return _appliedRules;
    }

    private void AppliedRule(int ruleNumber)
    {
        _appliedRules.Add(ruleNumber);
    }

    private void Next()
    {
        if (_currentSymbolIndex >= _input.Length)
        {
            return;
        }

        _currentSymbolIndex++;
    }

    private void Eat(char token)
    {
        if (CurrentToken == token)
        {
            Next();
        }
        else
        {
            throw new Exception($"Expected '{token}' but got '{CurrentToken}' at position {_currentSymbolIndex}");
        }
    }

    private void ThrowUnexpectedToken()
    {
        throw new Exception($"Unexpected token {CurrentToken} at position {_currentSymbolIndex}");
    }

    private void S()
    {
        switch (CurrentToken)
        {
            case 'a':
                AppliedRule(1);
                Eat('a');
                N2();
                Eat(';');
                N1();
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void N1()
    {
        switch (CurrentToken)
        {
            case 'a':
                AppliedRule(2);
                Eat('a');
                N2();
                Eat(';');
                N1();
                break;
            case '$' or ']':
                AppliedRule(3);
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void N2()
    {
        switch (CurrentToken)
        {
            case '[':
                AppliedRule(4);
                Eat('[');
                S();
                Eat(']');
                N3();
                break;
            case '=':
                AppliedRule(5);
                Eat('=');
                E();
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void N3()
    {
        switch (CurrentToken)
        {
            case '[':
                AppliedRule(6);
                Eat('[');
                S();
                Eat(']');
                break;
            case ';':
                AppliedRule(7);
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void E()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(8);
                Eat('(');
                E();
                Eat(')');
                T_Mark();
                E_Mark();
                break;
            case '-':
                AppliedRule(9);
                Eat('-');
                Eat('(');
                E();
                Eat(')');
                T_Mark();
                E_Mark();
                break;
            case 'a':
                AppliedRule(10);
                Eat('a');
                T_Mark();
                E_Mark();
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void E_Mark()
    {
        switch (CurrentToken)
        {
            case '+':
                AppliedRule(11);
                Eat('+');
                T();
                E_Mark();
                break;
            case '$' or ')' or ';':
                AppliedRule(12);
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void T()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(13);
                Eat('(');
                E();
                Eat(')');
                T_Mark();
                break;
            case '-':
                AppliedRule(14);
                Eat('-');
                Eat('(');
                E();
                Eat(')');
                T_Mark();
                break;
            case 'a':
                AppliedRule(15);
                Eat('a');
                T_Mark();
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void T_Mark()
    {
        switch (CurrentToken)
        {
            case '*':
                AppliedRule(16);
                Eat('*');
                P();
                T_Mark();
                break;
            case '$' or '+' or ')' or ';':
                AppliedRule(17);
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }

    private void P()
    {
        switch (CurrentToken)
        {
            case '(':
                AppliedRule(18);
                Eat('(');
                E();
                Eat(')');
                break;
            case '-':
                AppliedRule(19);
                Eat('-');
                Eat('(');
                E();
                Eat(')');
                break;
            case 'a':
                AppliedRule(20);
                Eat('a');
                break;
            default:
                ThrowUnexpectedToken();
                break;
        }
    }
}