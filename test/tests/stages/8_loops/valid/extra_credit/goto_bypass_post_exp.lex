-- Lexing ... OK
+
+
@@ Tokens @@
List[54]:
  int
  identifier(main)
  (
  void
  )
  {
  int
  identifier(sum)
  =
  const int(0)
  ;
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  ;
  identifier(i)
  =
  const int(0)
  )
  {
  identifier(lbl)
  :
  identifier(sum)
  =
  identifier(sum)
  +
  const int(1)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  >
  const int(10)
  )
  break
  ;
  goto
  identifier(lbl)
  ;
  }
  return
  identifier(sum)
  ;
  }
