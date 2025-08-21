-- Lexing ... OK
+
+
@@ Tokens @@
List[37]:
  union
  identifier(u)
  {
  int
  identifier(a)
  ;
  long
  identifier(b)
  ;
  }
  ;
  struct
  identifier(s)
  {
  int
  identifier(tag)
  ;
  union
  identifier(u)
  identifier(contents)
  ;
  }
  ;
  struct
  identifier(s)
  identifier(my_struct)
  =
  {
  const int(10)
  ,
  {
  const int(1)
  ,
  const int(2)
  }
  }
  ;
