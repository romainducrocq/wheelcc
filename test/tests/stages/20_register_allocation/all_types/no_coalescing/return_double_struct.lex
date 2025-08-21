-- Lexing ... OK
+
+
@@ Tokens @@
List[190]:
  struct
  identifier(s)
  {
  double
  identifier(d1)
  ;
  double
  identifier(d2)
  ;
  }
  ;
  int
  identifier(global_one)
  =
  const int(1)
  ;
  struct
  identifier(s)
  identifier(return_struct)
  (
  void
  )
  {
  int
  identifier(one)
  =
  const int(2)
  -
  identifier(global_one)
  ;
  int
  identifier(two)
  =
  identifier(one)
  +
  identifier(one)
  ;
  int
  identifier(three)
  =
  const int(2)
  +
  identifier(one)
  ;
  int
  identifier(four)
  =
  identifier(two)
  *
  identifier(two)
  ;
  int
  identifier(five)
  =
  const int(6)
  -
  identifier(one)
  ;
  int
  identifier(six)
  =
  identifier(two)
  *
  identifier(three)
  ;
  int
  identifier(seven)
  =
  identifier(one)
  +
  const int(6)
  ;
  int
  identifier(eight)
  =
  identifier(two)
  *
  const int(4)
  ;
  int
  identifier(nine)
  =
  identifier(three)
  *
  identifier(three)
  ;
  int
  identifier(ten)
  =
  identifier(four)
  +
  identifier(six)
  ;
  int
  identifier(eleven)
  =
  const int(16)
  -
  identifier(five)
  ;
  int
  identifier(twelve)
  =
  identifier(six)
  +
  identifier(six)
  ;
  if
  (
  identifier(one)
  ==
  const int(1)
  &&
  identifier(two)
  ==
  const int(2)
  &&
  identifier(three)
  ==
  const int(3)
  &&
  identifier(four)
  ==
  const int(4)
  &&
  identifier(five)
  ==
  const int(5)
  &&
  identifier(six)
  ==
  const int(6)
  &&
  identifier(seven)
  ==
  const int(7)
  &&
  identifier(eight)
  ==
  const int(8)
  &&
  identifier(nine)
  ==
  const int(9)
  &&
  identifier(ten)
  ==
  const int(10)
  &&
  identifier(eleven)
  ==
  const int(11)
  &&
  identifier(twelve)
  ==
  const int(12)
  )
  {
  struct
  identifier(s)
  identifier(retval)
  =
  {
  const double(0.0)
  ,
  const double(200.0)
  }
  ;
  return
  identifier(retval)
  ;
  }
  else
  {
  struct
  identifier(s)
  identifier(retval)
  =
  {
  const double(1.0)
  ,
  -
  const double(1.0)
  }
  ;
  return
  identifier(retval)
  ;
  }
  }
