fn main() {
    let mut a = 10;
    while a > 0 {
       a -= 1;

       if a == 5 {
           return;
       }
       println!(a);
    }
}
