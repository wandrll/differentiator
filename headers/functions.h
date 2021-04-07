#include <stdio.h>

FUNCTION(sin, 0, {
    return _MUL(nodecos(cRight), dRight);
})

FUNCTION(log, 1, {
    return _MUL(_DIV(_NODE_BY_VAL(1), cRight), dRight);
})

FUNCTION(cos, 2, {
    return _MUL(_MUL(_NODE_BY_VAL(-1), nodesin(cRight)), dRight);
})

FUNCTION(tan, 3, {
    return _MUL(_DIV(_NODE_BY_VAL(1), _POW(nodecos(cRight), _NODE_BY_VAL(2))), dRight);
})

FUNCTION(asin, 4, {
    return _MUL(_DIV(_NODE_BY_VAL(1), _POW(_MINUS(_NODE_BY_VAL(1), _POW(cRight, _NODE_BY_VAL(2))), _DIV(_NODE_BY_VAL(1), _NODE_BY_VAL(2)))), dRight);
})

FUNCTION(ctan, 5, {
    return _MUL(_DIV(_NODE_BY_VAL(-1), _POW(nodesin(cRight), _NODE_BY_VAL(2))), dRight);
})