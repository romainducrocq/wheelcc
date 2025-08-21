-- Lexing ... OK
+
+
@@ Tokens @@
List[363]:
  int
  identifier(check_one_int)
  (
  int
  identifier(actual)
  ,
  int
  identifier(expected)
  )
  ;
  int
  identifier(check_5_ints)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  int
  identifier(start)
  )
  ;
  int
  identifier(check_12_ints)
  (
  int
  identifier(a)
  ,
  int
  identifier(b)
  ,
  int
  identifier(c)
  ,
  int
  identifier(d)
  ,
  int
  identifier(e)
  ,
  int
  identifier(f)
  ,
  int
  identifier(g)
  ,
  int
  identifier(h)
  ,
  int
  identifier(i)
  ,
  int
  identifier(j)
  ,
  int
  identifier(k)
  ,
  int
  identifier(l)
  ,
  int
  identifier(start)
  )
  ;
  int
  identifier(id)
  (
  int
  identifier(x)
  )
  ;
  int
  identifier(glob0)
  =
  const int(0)
  ;
  int
  identifier(glob1)
  =
  const int(1)
  ;
  int
  identifier(glob2)
  =
  const int(2)
  ;
  int
  identifier(glob3)
  =
  const int(3)
  ;
  int
  identifier(glob4)
  =
  const int(4)
  ;
  int
  identifier(glob5)
  =
  const int(5)
  ;
  int
  identifier(reset_globals)
  (
  void
  )
  {
  identifier(glob0)
  =
  const int(0)
  ;
  identifier(glob1)
  =
  const int(0)
  ;
  identifier(glob2)
  =
  const int(0)
  ;
  identifier(glob3)
  =
  const int(0)
  ;
  identifier(glob4)
  =
  const int(0)
  ;
  identifier(glob5)
  =
  const int(0)
  ;
  return
  const int(0)
  ;
  }
  int
  identifier(flag)
  =
  const int(1)
  ;
  int
  identifier(target)
  (
  void
  )
  {
  int
  identifier(a)
  =
  identifier(glob0)
  ;
  int
  identifier(b)
  =
  identifier(glob1)
  ;
  int
  identifier(c)
  =
  identifier(glob2)
  ;
  int
  identifier(d)
  =
  identifier(glob3)
  ;
  int
  identifier(e)
  =
  identifier(glob4)
  ;
  int
  identifier(f)
  ;
  int
  identifier(g)
  ;
  int
  identifier(h)
  ;
  int
  identifier(i)
  ;
  int
  identifier(j)
  ;
  if
  (
  identifier(flag)
  )
  {
  identifier(reset_globals)
  (
  )
  ;
  identifier(f)
  =
  identifier(a)
  ;
  identifier(check_one_int)
  (
  identifier(a)
  ,
  const int(0)
  )
  ;
  identifier(g)
  =
  identifier(b)
  ;
  identifier(check_one_int)
  (
  identifier(b)
  ,
  const int(1)
  )
  ;
  identifier(h)
  =
  identifier(c)
  ;
  identifier(check_one_int)
  (
  identifier(c)
  ,
  const int(2)
  )
  ;
  identifier(i)
  =
  identifier(d)
  ;
  identifier(check_one_int)
  (
  identifier(d)
  ,
  const int(3)
  )
  ;
  identifier(j)
  =
  identifier(e)
  ;
  identifier(check_one_int)
  (
  identifier(e)
  ,
  const int(4)
  )
  ;
  }
  else
  {
  identifier(e)
  =
  const int(0)
  ;
  identifier(f)
  =
  const int(0)
  ;
  identifier(g)
  =
  const int(0)
  ;
  identifier(h)
  =
  const int(0)
  ;
  identifier(i)
  =
  const int(0)
  ;
  identifier(j)
  =
  const int(0)
  ;
  }
  identifier(check_one_int)
  (
  identifier(f)
  ,
  const int(0)
  )
  ;
  identifier(check_one_int)
  (
  identifier(g)
  ,
  const int(1)
  )
  ;
  identifier(check_one_int)
  (
  identifier(h)
  ,
  const int(2)
  )
  ;
  identifier(check_one_int)
  (
  identifier(i)
  ,
  const int(3)
  )
  ;
  identifier(check_one_int)
  (
  identifier(j)
  ,
  const int(4)
  )
  ;
  identifier(check_one_int)
  (
  identifier(glob0)
  ,
  const int(0)
  )
  ;
  identifier(check_one_int)
  (
  identifier(glob1)
  ,
  const int(0)
  )
  ;
  identifier(check_one_int)
  (
  identifier(glob2)
  ,
  const int(0)
  )
  ;
  identifier(check_one_int)
  (
  identifier(glob3)
  ,
  const int(0)
  )
  ;
  identifier(check_one_int)
  (
  identifier(glob4)
  ,
  const int(0)
  )
  ;
  return
  const int(0)
  ;
  }
