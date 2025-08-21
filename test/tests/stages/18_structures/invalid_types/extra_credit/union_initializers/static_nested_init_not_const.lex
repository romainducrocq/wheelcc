-- Lexing ... OK
+
+
@@ Tokens @@
List[42]:
  union
  identifier(u)
  {
  long
  identifier(l)
  ;
  }
  ;
  struct
  identifier(has_union)
  {
  int
  identifier(a)
  ;
  union
  identifier(u)
  identifier(b)
  ;
  char
  identifier(c)
  ;
  }
  ;
  long
  identifier(some_var)
  =
  const long(10l)
  ;
  struct
  identifier(has_union)
  identifier(some_struct)
  =
  {
  const int(1)
  ,
  {
  identifier(some_var)
  }
  ,
  const char('a')
  }
  ;
