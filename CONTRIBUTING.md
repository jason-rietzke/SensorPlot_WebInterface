# How to contribute

I'm really glad you're reading this, because I am always happy when someone is interested in such projects.
Since this is a open source lib, you can use, modify and distribute it as you want.

If you want to make suggestions for improvements or think about changing something yourself I would be happy to include those features into the library.


## Submitting changes

Please send a [GitHub Pull Request to SensorPlot_WebInterface](https://github.com/jason-rietzke/SensorPlot_WebInterface/pull/new/master) with a clear list of what you've done (read more about pull requests). 
When you send a pull request, please follow my coding conventions (below) and make sure all of your commits are atomic (one feature per commit).

Always write a clear log message for your commits. One-line messages are fine for small changes, but bigger changes should look like this:

    $ git commit -m "A brief summary of the commit
    > 
    > A paragraph describing what changed and its impact."


## Coding conventions

Start reading the code and you'll get the hang of it. We optimize for readability:

  * I indent using tabs befor spaces
  * I avoid logic in views (might be not as strict in this project because of resource limitations of the environment it's running on [Microcontrollers])
  * This is open source software. Consider the people who will read your code, and make it look nice for them.
