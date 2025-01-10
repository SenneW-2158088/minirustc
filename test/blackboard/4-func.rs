fn factorial(n:i32) -> i32
{
  // if-expressions
  if n <= 0  { 1 }
  else { n * factorial(n-1) }
  // return van een functie is de laatste expressie in de body
}

fn main()
{
  let v = factorial(9);
  println!("{v}");
  let v = fact2(9);
  println!("{v}");
}

fn fact2(n:i32) -> i32
{
  let r:i32;
  r = if n <= 0  { 1 }
      else { n * factorial(n-1) };
  r // waarde van assignment is ()
}
