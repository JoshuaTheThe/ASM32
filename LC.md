# simple language
- called just lc

```pascal
PROCEDURE Secondary [a:Integer; b:Integer]
BEGIN
        [a + b];
END;

PROCEDURE Main [c:Integer; v:^^Char]
BEGIN
        DEFINE a:Integer;
        DEFINE b:Integer;
        SET [a; 65];
        SET [b; 64];
        Secondary [a; b];
END;
```

- all statements end in seperator
- only one seperator, the semi colon
- if the symbol is a procedure, call it with the args;
- the language // tokeniser is not case sensitive
- () is alias of []
