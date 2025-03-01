# Mah's Jong
### 607 Games: Walker Strauss, Patrick Choo, Keqing Jiang, Luna Lu, Tian Jin, Tina Lu, Aiden Keck, Mariia Tiulchenko, Alexander Balako

### Goal:
Reach 20 points before you run out of turns! For each turn you there are three actions you may perform: discard, make pairs, or play sets. In order to score points, you must play sets from your hand. Sets can look like the following: three of a kind, straight with three tiles, and four of a kind. Each set is worth exactly the number of tiles in it; however, some sets may activate combo points. The scoring looks like the following: 

* Same set (same tiles in two or more sets) -> rescore the first set <br>
* Consecutive suits (two or more that are strictly ascending from one set to another) -> rescore the second set <br>
* Mahjong Set (four 3 sets and a pair OR three 4 sets and a pair) -> rescore each set according to its position in played hand (first set * 1, second set * 2, ...)

In addition to scoring, you will also have the ability to gain wild tiles. If you play a hand that includes a "Grandma Tile" you will gain as many wild tiles as tiles you have played that are "Grandma Tiles". These tiles will have a rank, but can be played in combination with any suit. Wild tiles will not count as grandma tiles. 

For each turn, you are allowed to play up to 14 tiles, assuming they are all apart of a valid mahjong set. You are allowed up to 5 turns and 3 discards with a limit of 5 tiles per discard. You will always have up 14 tiles during each turn; when you make a pair, you must discard two tiles in your hand, and whenever you play a set you will recover the same amount from the pile. The pile will regenerate after a layer is gone. If you reaach 20 points, you win! If you fail to reach 20 points before 5 turns, you lose!

---
### Controls
#### Mouse Controls
Make pile pairs: In the pile, click two of the same tile to form a pair. Once the pair is selected, you must click two tiles in your hand to discard. (note: no discarding once you make pairs from the pile)
Select tiles: In your hand, select and deselect tiles by clicking on them.
#### Keyboard Controls
Discard: Press D to discard up to 5 selected cards (you cannot discard after making pile pairs or make pairs after discarding) <br>
Make set: Press S to select a set to be played (will not select if invalid, you must select sets you want to play individually) <br>
Play sets: Press P to play all selected sets (will only play sets that have been made by pressing S) <br>

###### Note: We are currently pivoting games; hence, some features we have discussesd in our write-ups may not reflect this prototype
###### Note: The application will build but there is a nullptr error that only occurs when making pairs on a Windows device. 

