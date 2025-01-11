fn main()
{
    let x = 100;
    let mut b = 5;
    let y =
  {
    let x = b;
    b += 10;
    x + 1;
  };

    println!(b);
    println!(x);
    println!(y);
}
