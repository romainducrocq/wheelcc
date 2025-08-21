-- Lexing ... OK
+
+
@@ Tokens @@
List[147]:
  struct
  identifier(s)
  {
  int
  identifier(a)
  ;
  int
  identifier(b)
  ;
  }
  ;
  int
  identifier(main)
  (
  void
  )
  {
  for
  (
  int
  identifier(i)
  =
  const int(0)
  ;
  identifier(i)
  <
  const int(10)
  ;
  identifier(i)
  =
  identifier(i)
  +
  const int(1)
  )
  {
  struct
  identifier(s)
  identifier(autom)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  static
  struct
  identifier(s)
  identifier(stat)
  =
  {
  const int(1)
  ,
  const int(2)
  }
  ;
  identifier(autom)
  .
  identifier(a)
  =
  identifier(autom)
  .
  identifier(a)
  +
  const int(1)
  ;
  identifier(autom)
  .
  identifier(b)
  =
  identifier(autom)
  .
  identifier(b)
  +
  const int(1)
  ;
  identifier(stat)
  .
  identifier(a)
  =
  identifier(stat)
  .
  identifier(a)
  +
  const int(1)
  ;
  identifier(stat)
  .
  identifier(b)
  =
  identifier(stat)
  .
  identifier(b)
  +
  const int(1)
  ;
  if
  (
  identifier(i)
  ==
  const int(9)
  )
  {
  if
  (
  identifier(stat)
  .
  identifier(a)
  !=
  const int(11)
  ||
  identifier(stat)
  .
  identifier(b)
  !=
  const int(12)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(autom)
  .
  identifier(a)
  !=
  const int(2)
  ||
  identifier(autom)
  .
  identifier(b)
  !=
  const int(3)
  )
  {
  return
  const int(2)
  ;
  }
  }
  }
  return
  const int(0)
  ;
  }
