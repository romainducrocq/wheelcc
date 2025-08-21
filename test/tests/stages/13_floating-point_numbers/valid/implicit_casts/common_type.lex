-- Lexing ... OK
+
+
@@ Tokens @@
List[170]:
  int
  identifier(lt)
  (
  double
  identifier(d)
  ,
  long
  identifier(l)
  )
  {
  return
  identifier(d)
  <
  identifier(l)
  ;
  }
  double
  identifier(tern_double_flag)
  (
  double
  identifier(flag)
  )
  {
  return
  (
  double
  )
  (
  identifier(flag)
  ?
  -
  const int(30)
  :
  const unsigned long(10ul)
  )
  ;
  }
  double
  identifier(tern_double_result)
  (
  int
  identifier(flag)
  )
  {
  return
  identifier(flag)
  ?
  const double(5.0)
  :
  const unsigned long(9223372036854777850ul)
  ;
  }
  int
  identifier(ten)
  =
  const int(10)
  ;
  int
  identifier(multiply)
  (
  void
  )
  {
  int
  identifier(i)
  =
  const double(10.75)
  *
  identifier(ten)
  ;
  return
  identifier(i)
  ==
  const int(107)
  ;
  }
  int
  identifier(main)
  (
  void
  )
  {
  if
  (
  identifier(lt)
  (
  -
  const double(9007199254751228.0)
  ,
  -
  const long(9007199254751227l)
  )
  )
  {
  return
  const int(1)
  ;
  }
  if
  (
  identifier(tern_double_flag)
  (
  const double(20.0)
  )
  !=
  const double(18446744073709551586.0)
  )
  {
  return
  const int(2)
  ;
  }
  if
  (
  identifier(tern_double_flag)
  (
  const double(0.0)
  )
  !=
  const double(10.0)
  )
  {
  return
  const int(3)
  ;
  }
  if
  (
  identifier(tern_double_result)
  (
  const int(1)
  )
  !=
  const double(5.0)
  )
  {
  return
  const int(4)
  ;
  }
  if
  (
  identifier(tern_double_result)
  (
  const int(0)
  )
  !=
  const double(9223372036854777856.0)
  )
  {
  return
  const int(5)
  ;
  }
  if
  (
  !
  identifier(multiply)
  (
  )
  )
  {
  return
  const int(6)
  ;
  }
  return
  const int(0)
  ;
  }
