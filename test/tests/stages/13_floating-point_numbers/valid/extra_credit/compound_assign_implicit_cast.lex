-- Lexing ... OK
+
+
@@ Tokens @@
List[71]:
  int
  identifier(main)
  (
  void
  )
  {
  double
  identifier(d)
  =
  const double(1000.5)
  ;
  identifier(d)
  +=
  const int(1000)
  ;
  if
  (
  identifier(d)
  !=
  const double(2000.5)
  )
  {
  return
  const int(1)
  ;
  }
  unsigned
  long
  identifier(ul)
  =
  const unsigned long(18446744073709551586ul)
  ;
  identifier(ul)
  -=
  const double(1.5E19)
  ;
  if
  (
  identifier(ul)
  !=
  const unsigned long(3446744073709551616ul)
  )
  {
  return
  const int(2)
  ;
  }
  int
  identifier(i)
  =
  const int(10)
  ;
  identifier(i)
  +=
  const double(0.99999)
  ;
  if
  (
  identifier(i)
  !=
  const int(10)
  )
  {
  return
  const int(3)
  ;
  }
  return
  const int(0)
  ;
  }
