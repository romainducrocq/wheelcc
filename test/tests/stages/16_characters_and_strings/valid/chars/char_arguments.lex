-- Lexing ... OK
+
+
@@ Tokens @@
List[240]:
  int
  identifier(check_args)
  (
  char
  identifier(a)
  ,
  signed
  char
  identifier(b)
  ,
  char
  identifier(c)
  ,
  unsigned
  char
  identifier(d)
  ,
  char
  identifier(e)
  ,
  char
  identifier(f)
  ,
  signed
  char
  identifier(g)
  ,
  char
  identifier(h)
  )
  {
  char
  identifier(expected_a)
  =
  const int(5)
  ;
  signed
  char
  identifier(expected_b)
  =
  -
  const int(12)
  ;
  char
  identifier(expected_c)
  =
  const int(117)
  ;
  unsigned
  char
  identifier(expected_d)
  =
  const int(254)
  ;
  char
  identifier(expected_e)
  =
  const int(1)
  ;
  char
  identifier(expected_f)
  =
  -
  const int(20)
  ;
  signed
  char
  identifier(expected_g)
  =
  const int(60)
  ;
  char
  identifier(expected_h)
  =
  const int(100)
  ;
  if
  (
  identifier(expected_a)
  !=
  identifier(a)
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(expected_b)
  !=
  identifier(b)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(expected_c)
  !=
  identifier(c)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(expected_d)
  !=
  identifier(d)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(expected_e)
  !=
  identifier(e)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  identifier(expected_f)
  !=
  identifier(f)
  )
  {
  return
  const int(6)
  ;
  }
  if
  (
  identifier(expected_g)
  !=
  identifier(g)
  )
  {
  return
  const int(7)
  ;
  }
  if
  (
  identifier(expected_h)
  !=
  identifier(h)
  )
  {
  return
  const int(8)
  ;
  }
  return
  const int(0)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  char
  identifier(a)
  =
  const int(5)
  ;
  signed
  char
  identifier(b)
  =
  -
  const int(12)
  ;
  char
  identifier(c)
  =
  const int(117)
  ;
  unsigned
  char
  identifier(d)
  =
  const int(254)
  ;
  char
  identifier(e)
  =
  const int(1)
  ;
  char
  identifier(f)
  =
  -
  const int(20)
  ;
  signed
  char
  identifier(g)
  =
  const int(60)
  ;
  char
  identifier(h)
  =
  const int(100)
  ;
  return
  identifier(check_args)
  (
  identifier(a)
  ,
  identifier(b)
  ,
  identifier(c)
  ,
  identifier(d)
  ,
  identifier(e)
  ,
  identifier(f)
  ,
  identifier(g)
  ,
  identifier(h)
  )
  ;
  }
