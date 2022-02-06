Wordle Helper
===

This is a Python3 script that helps you solve the [Wordle](https://www.powerlanguage.co.uk/wordle/) puzzles created by Josh Wardle.

## Usage

Program first displays the relative frequency of individual letters in the embedded dictionary of five-letter words. Based on the probability it will suggest initial try words. Afterwards you are required to input your actual Wordle input together with the colored Wordle response indicating matches. Input a five digit number where "0" indicates absent letter (gray background), "1" stands for a matching letter but not in position (yellow background), and "2" for a match in position. Based on this response the program will suggest most probable words. The program also suggests anti-matches so that at the beggining you might try to exclude as many letters as possible and reduce the pool of possible matches for the next guess.

## License

Released under the GPL version 3.
