# **Monte Carlo Tree Search VS MiniMax for a Chess AI**

## **Introduction**
Welcome to the repository for my research project conducted as part of the **DAE program** at the **University of Howest**. The primary objective of this project was to explore and compare two distinct algorithms, **Monte Carlo Tree Search (MCTS)** and **MiniMax with Alpha-Beta pruning**, in the context of developing Chess AI. The project started with a provided Win32 engine from our **DAE Teachers**. This ReadMe aims to provide an overview of the project, its goals, methods, findings, and resources utilized during the research.

## **Structure**
1. **Short Chess AI History**
2. **Differences between MCTS and MiniMax**
3. **How does a board position get evaluated?**
4. **Comparison Methodology**
5. **Results and Analysis**
6. **Conclusion**

## **Short Chess AI History**
Until around 2018, almost all top-level Chess Engines used the MiniMax (with Alpha-Beta pruning) algorithm. This changed with the creation of AlphaZero (by DeepMind, later acquired by Google). AlphaZero quickly rose to the top of Chess Engines using the Monte Carlo Tree Search algorithm. With this research project, I aimed to test and compare Monte Carlo Tree Search VS MiniMax with Alpha-Beta pruning.

## **Differences between MCTS and MiniMax**
Starting this project posed two challenges: constructing the entire game of chess from scratch and subsequently creating two Chess AI models based on different algorithms. The starting point required delving into the mechanics of chess, deciphering its rules, and engineering the game's framework. This initial phase, although not the primary focus, demanded substantial effort and attention to detail.

The goal of this project, however, lies in the differences between **Monte Carlo Tree Search (MCTS)** and **MiniMax with Alpha-Beta pruning** algorithms for Chess AI. Here these two algorithms are explained:

### **MiniMax with Alpha-Beta pruning:**

<div align="center">
	<img src="https://github.com/Howest-DAE-GD/gpp-researchtopic-RobbeHijzen/blob/main/ChessEngine_Luan/Resources/GIFs/GIF_02.gif" width="400" height="400" />
 <p>How MiniMax loops over possible moves (Depth = 2)</p>
</div>


- A depth-first searching algorithm that, as the name implies, minimises the best move for the opponent, while maximising your own best moves.
- The algorithm looks into a certain 'depth' (amout of moves into the future).
- For every possible final node (board position) at the given depth, a value for the board position is generated. If this value is negative it's in favor of the enemy and if the value is positive it's in favor of yourself (NOTE: Only the relative value to other board positions matter so some Engines don't go negative at all).



<div align="center">
	<img src="https://github.com/Howest-DAE-GD/gpp-researchtopic-RobbeHijzen/blob/main/ChessEngine_Luan/Resources/GIFs/GIF_01.gif" width="1000" height="350" />
 <p>How MiniMax assigns values to nodes (Depth = 2)</p>
</div>

- With the 'Leaf Nodes' having a value now, we can go back up the tree and assign a value to the parent node above. Since the AI only moves every other move however, this is done in two ways:
  1. If the parent node would be the move for the AI: The **Maximum** value of the child nodes will be assigned to the parent node.
  2. If the parent node would be the move for the opponent: The **Minimum** value of the child nodes will be assigned to the parent node.

This is logical because if it's your turn to move, you will pick the best move for that position, but if it's the opponent's turn, they will (probably) pick the move which is best for them, meaning worst for you.


<div align="center">
	<img src="https://github.com/Howest-DAE-GD/gpp-researchtopic-RobbeHijzen/blob/main/ChessEngine_Luan/Resources/GIFs/GIF_03.gif" width="1000" height="350" />
 <p>How MiniMax calculates best move from known values</p>
</div>


A very important consideration for a Chess AI is the speed at which it operates. The faster it is, the deeper it can look into the future, which leads to better play. One way to optimize the MiniMax algorithm without losing quality is called Alpha-Beta pruning. I won't go into detail about this subject here, since it's not the goal of the research, but it comes down to skipping the evaluation of certain moves which won't have an impact on the game.


### **Monte Carlo Tree Search (MCTS):**
- Rather than looking at every possible board position from a certain depth, MCTS will only look at moves which **seem** interesting.
- MCTS doesn't use a certain depth like MiniMax does, instead uses an amount of times the algorithm will be performed, called iterations.
- Which moves to check depend on an expression which looks like this:

 $UCB1 = nodeValue/nodeVisits + C *$ $\sqrt{ln(parentNodeVisits) / nodeVisits} $

 This might seem like a strange expression, but it does make sense. The first part (before the '+') represents the 'Exploitation Term' and behind that is called the 'Exploration Term'. 
 
 The Exploitation Term is higher when the average value of the node is higher. The Exploration Term is higher when the node hasn't been visited often. The Constant 'C' is there to balance the Exploitation and Exploration Terms (with a higher C, a higher 'weight' for the Exploration Term).

The highest UCB1 value of all the children of the node being evaluated, will be picked next. This repeats until a node has been found which hasn't been explored yet (nodeVisits == 0).

There is much more to be said about this algorithm, but there are so many different ways to implement this that going in depth would only be confusing. 

 
<div align="center">
	<img src="https://github.com/Howest-DAE-GD/gpp-researchtopic-RobbeHijzen/blob/main/ChessEngine_Luan/Resources/GIFs/GIF_04.gif" width="400" height="400" />
 <p>How MCTS loops over possible moves.
	 This might seem rather random, which it sort of is.</p>
</div>

## **How does a board position get evaluated?**

One of the most important aspects of how good a Chess AI is, depends on how a certain board position gets a value. Before I start explaining how I made the evaluation function for this, I want to tell you that both AIs use the same evaluation function for fairness, apart from some **minor** tweaking to the weights of certain aspects. This is done because although I want to keep the game as fair as possible, the two algorithms have their strengths and weaknesses so I decided to have both AIs play to their strengths to display what such an AI can achieve.

### Evaluations for Board Valuation

During the evaluation of the board positions, the following four main criteria were considered:

#### 1. Material Balance:
- **Explanation:** Assigning values to chess pieces to determine their worth in the game.
- **Example:** A pawn holds a value of 1 point, while a queen is valued at 9 points.

#### 2. Other Material Considerations:
- **Explanation:** Considering the significance of specific material configurations beyond the simple value of pieces.
- **Example:** For instance, acknowledging that having two bishops might be more advantageous than having two knights. Also, two rooks might not synergize effectively.

#### 3. Pawn Structure:
- **Explanation:** Assessing the arrangement and coordination of pawns on the board.
- **Example:** A strong pawn structure might involve having four pawns interconnected and supporting each other on adjacent files, contrasting with a weaker structure where the same number of pawns are scattered across the board.

#### 4. Mobility:
- **Explanation:** Gauging the number of possible moves available in a given position.
- **Example:** Generally, positions with more available moves tend to be valued higher due to increased flexibility and potential options.

While several other evaluations such as King Safety or Board Control could significantly impact board valuation, due to time constraints, these four criteria were primarily focused on.

## **Comparison Methodology**
The way this comparison is measured is quite simple: I will let the two AIs (MCTS and MiniMax) battle it out for 250, and see how many times what algorithm wins from the other.

One note however is the time each side has. Time is very important when it comes to AIs battling each other, so I decided to change the variables (the depth for MiniMax and the amount of iterations for MCTS) so each side took roughly the same time for fairness. 


## **Results and Analysis**
Following an extensive series of 250 chess games between the MiniMax algorithm with Alpha-Beta pruning and the Monte Carlo Tree Search (MCTS), a distinct trend surfaced: MiniMax emerged as the dominant force. Out of these encounters, MiniMax secured victory in 209 games, witnessed 34 draws, and the remaining 7 games were claimed by MCTS.

The big difference in victories can be attributed to the following observations during gameplay. The MCTS algorithm, although explosive in certain scenarios, showed a tendency to make big blunders. In specific instances, it decided to simply sacrifice the queen for a pawn, failing to consider crucial enemy moves within the game.

The heuristic nature of the MCTS algorithm led to occasional suboptimal moves, highlighting areas for potential improvement. Addressing this issue might involve further optimization of algorithmic variables, potentially through the integration of reinforcement learning techniques. However, such a project was beyond the scope of this research. I have made efforts to tweak the variables in a favorable way towards MCTS, but it seems to not matter much for the outcome

Conversely, the MiniMax algorithm showed a remarkable consistency in decision-making. Armed with the ability to calculate the best moves for a certain depth, MiniMax showed an exceptional understanding of the game's possible moves. Its deterministic approach significantly reduced the amount of blunders, but also having a less explosive playstyle.

## **Conclusion**
In conclusion, it is clear that my MCTS AI was outclassed by the MiniMax algorithm by a tremendous amount. The MCTS algorithm displayed adaptability and exploration of varied game scenarios, but makes critical blunders often due to the heuristic nature. The goal of the MCTS is that it can rule out entire branches so it's able to see other more valuable branches more in detail. The problem with this however is that it can rule out very important moves that sometimes leads to checkmate.

The MiniMax algorithm on the other hand is not heuristic at all, in the sense that it knows the exact best move for a given depth (considering the Evaluation function of course). This means that blunders are very rare, but it also increases the search time. This tradeoff didn't seem to matter in this research however.

Do note however that this Research Project bares very little scientific value. It was fun to see what the strengths and weaknesses of each algorithm was even though it might not be an accurate representation of the actual strengths of the algorithms.

## **Sources**
https://www.chessprogramming.org/Main_Page																																
											
https://chessify.me/blog/top-chess-engines

https://towardsdatascience.com/alphazero-chess-how-it-works-what-sets-it-apart-and-what-it-can-tell-us-4ab3d2d08867

https://astrakhan.consulting/blog/analysis-of-artificial-intelligence-in-chess/#:~:text=the%20long%20run.-,Chess%20engine%20algorithms,algorithm%20and%20alpha-beta%20pruning.
