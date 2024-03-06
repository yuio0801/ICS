#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
//#include "elf.c"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

extern uint32_t look_up_symtab(char *sym, bool *success);

enum
{
	NOTYPE = 256,
	EQ,
	NUM,
	REG,
	SYMB,
    ADD,
    SUB,
    MUL,
    DIV,
    LeftB,
    RightB,
    REGISTER,
    EL,
    NEL,
    ABE,
    BLE,
    AB,
    BL,
    AND,
    OR,
    NOT,
    DEREF,
    NE,
    EXPR,
    
	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space
	{"\\+", ADD},
	{"-", SUB},
	{"\\*", MUL},
	{"\\/", DIV},
	{"0x[0-9a-f]+", NUM},
	{"\\(", LeftB},
	{"\\)", RightB},
	{"\\$[a-z]{3}", REGISTER},
	{"==", EL},
	{"!=", NEL},
	{"<=", BLE},
	{">=", ABE},
	{">", AB},
	{"<", BL},
	{"&&", AND},
	{"\\|\\|", OR},
	{"!", NOT},
	{"[a-zA-Z][a-zA-Z0-9_]*", EXPR}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;
	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				    case NOTYPE:
				        break;
				    case NUM:
				        if(substr_len > 32)
				        {
				            printf("number out of range.\n");
				            return false;
				        }
				        for(int k=0; k<substr_len; ++k)
				        {
				            tokens[nr_token].str[k] = substr_start[k];
				        }
				        tokens[nr_token].str[substr_len] = '\0';
				    case REGISTER:
				        for(int k=0; k<substr_len; ++k)
				        {
				            tokens[nr_token].str[k] = substr_start[k];
				        }
				        tokens[nr_token].str[substr_len] = '\0';
				    case EXPR:
				        for(int k=0; k<substr_len; ++k)
				        {
				            tokens[nr_token].str[k] = substr_start[k];
				        }
				        tokens[nr_token].str[substr_len] = '\0';
				    default:
					    tokens[nr_token].type = rules[i].token_type;
					    nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}
static bool check_parentheses(int p, int q)
{
    int* st = (int *)malloc(sizeof(int) * nr_token);
    int top = 0;
    for(int i = p; i < q; ++i)
    {
        if(tokens[i].type == RightB)
        {
            if(top && tokens[st[top]].type == LeftB)
            {
                top--;
            }
            else
            {
                return false;
            }
        }
        else if(tokens[i].type == LeftB)
        {
            st[++top] = i;
        }
    }
    if(top == 1 && st[1] == p && tokens[q].type == RightB && tokens[st[1]].type == LeftB)
    {
        return true;
    }
    else
    {
        return false;
    }
}

typedef struct {
    bool check;
    uint32_t val;
}Res;

Res calc(int p)
{
    /*uint32_t val = 0;
    uint32_t base = 16;
    for(int i = 2; tokens[p].str[i] != '\0'; i++)
    {
        if(val >> 31 == 1)
        {
            return (Res){false, -1};
        }
        val = val * base + (tokens[p].str[i] - '0');
    }
    return (Res){true, val};*/
    uint32_t val = 0;
    if(sscanf(tokens[p].str, "%x", &val) == 1)
    {
        return (Res){true, val};
    }
    else
    {
        return (Res){false, -1};
    }
}
Res deco(int p)
{
    bool success = true;
    int val = get_reg_val(tokens[p].str + 1, &success);
    if(success)
    {
        return (Res){true, val};
    }
    else
    {
        return (Res){false, -1};
    }
}
bool is_operator(int op)
{
    if(op == ADD || op == SUB || op == MUL || op == DIV ||
       op == EL || op == NEL || op == AB || op == BL ||
       op == ABE || op == BLE || op == NOT || op == DEREF ||
       op == AND || op == OR || op == NE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int precedence(int op)
{
    if(op == DEREF || op == NOT || op == NE) return 2;
    if(op == MUL || op == DIV) return 3;
    if(op == ADD || op == SUB) return 4;
    if(op == BL || op == BLE || op == AB || op == ABE) return 5;
    if(op == EL || op == NEL) return 6;
    if(op == AND) return 11;
    if(op == OR) return 12;
    
    return 0;
}

int find_dominant(int p, int q)
{
    int pos = -1;
    int* st = (int *)malloc(sizeof(int) * nr_token);
    int top = 0;
    for(int i = q; i >= p; --i)
    {
        if(tokens[i].type == LeftB)
        {
            if(top && tokens[st[top]].type == RightB)
            {
                top--;
            }
        }
        else if(tokens[i].type == RightB)
        {
            st[++top] = i;
        }
        else
        {
            if(!top && is_operator(tokens[i].type))
            {
                if(pos == -1 || precedence(tokens[i].type) > precedence(tokens[pos].type))
                {
                    pos = i;
                }
            }
        }
    }
    return pos;
}
static Res eval(int p, int q) {
    if(p > q) {
        /* Bad expression */
        return (Res){false, -1};
    }
    else if(p == q) { 
        /* Single token.
         * For now this token should be a number. 
         * Return the value of the number.
         */ 
         bool success = false;
         uint32_t addr;
         switch(tokens[p].type) {
            case NUM: return calc(p); break;
            case REGISTER: 
                return deco(p); break;
            case EXPR:
                //printf("%s",tokens[p].str);
                addr = look_up_symtab(tokens[p].str, &success);
                if(success)
                {
                    
                    return (Res){true, addr};
                }
                else
                {
                    return (Res){false, -1};
                }
                /*if(success)
                {
                    return (Res){true, addr};
                }
                else
                {
                    return (Res){false, -1};
                }
                break;
                */
                
                
                
            default: assert(0);
         }
    }
    else if(p + 1 == q){
        Res res = eval(q, q);
        uint32_t val = res.val;
        if(!res.check)
        {
            return (Res){false, -1};
        }
        switch(tokens[p].type) {
            case NOT: return (Res){true, !val}; break;
            case NE: return (Res){true, -val}; break;
            case DEREF:
                //val = vaddr_read(val, 0, 4);
                val = instr_fetch(val, 4);
            return (Res){true, val}; break;
            default:
                assert(0);
            
        }
    }
    else if(check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses. 
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1); 
    }
    else {
        int op = find_dominant(p, q);
        if(op == -1)
        {
            return (Res){false, -1};
        }
        Res res1 = eval(p, op - 1);
        Res res2 = eval(op + 1, q);
        uint32_t val1 = res1.val;
        uint32_t val2 = res2.val;
        if(!res1.check || !res2.check)
        {
            return (Res){false, -1};
        }
        switch(tokens[op].type) {
            case ADD: return (Res){true, val1 + val2}; break;
            case SUB: return (Res){true, val1 - val2}; break;
            case MUL: return (Res){true, val1 * val2}; break;
            case DIV: return (Res){true, val1 / val2}; break;
            case BL: return (Res){true, val1 < val2}; break;
            case BLE: return (Res){true, val1 <= val2}; break;
            case AB: return (Res){true, val1 > val2}; break;
            case ABE: return (Res){true, val1 >= val2}; break;
            case EL: return (Res){true, val1 == val2}; break;
            case NEL: return (Res){true, val1 != val2}; break;
            case AND: return (Res){true, val1 && val2}; break;
            case OR: return (Res){true, val1 || val2}; break;
            default: assert(0);
        }
    }
    return (Res){false, -1};
}
uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
	/* TODO: Implement code to evaluate the expression. */
    for(int i = 0; i < nr_token; i ++) {
        if(tokens[i].type == MUL && (i == 0 || is_operator(i - 1) || tokens[i - 1].type == LeftB)) {
            tokens[i].type = DEREF;
        }
        else if(tokens[i].type == SUB && (i == 0 || is_operator(i - 1) || tokens[i - 1].type == LeftB)) {
            tokens[i].type = NE;
        }
    }
    Res res = eval(0, nr_token - 1);
    if(!res.check)
    {
        printf("invalid expression");
        return -1;
    }
    else
    {
        return res.val;   
    }
}
