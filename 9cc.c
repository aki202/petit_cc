#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token kinds
typedef enum
{
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 終端トークン
} TokenKind;

typedef struct Token Token;

// Token type
struct Token
{
  TokenKind kind; // Token kind
  Token *next;    // Next input token
  int val;        // number if kind == TK_NUM
  char *str;      // String
};

// Token which is being watched.
Token *token;

// Function to report error, which get the format as same as printf.
void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// If next token is expected one, proceed curent token and return true.
// Otherwise, return false.
bool consume(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// If next token is expected one, proceed current token.
// Otherwise, report error.
void expect(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
  {
    error("'%c'ではありません", op);
  }
  token = token->next;
}

// If next token is number, proceed current token and return the number.
// Otherwise, report error.
int expect_number()
{
  if (token->kind != TK_NUM)
    error("Not number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->kind == TK_EOF;
}

// Create token, and connect it to cur.
Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// Return token which is tokenized from inputted p.
Token *tokenize(char *p)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    // Skip if it's space.
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-')
    {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("Cannot tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "Invalid number of arguments\n");
    return 1;
  }

  // Tokenize
  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number());

  while (!at_eof())
  {
    if (consume('+'))
    {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    consume('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}
