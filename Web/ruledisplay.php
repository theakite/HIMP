<!DOCTYPE html>
<html>
    <head>
        <title>Rules for Room <?php print $_GET['room']?></title>
    </head>

    <body>
        <?php
            if(file_exists('./parameter.xml')) {
                $doc = new DOMDocument();
                $doc->load('parameter.xml');

                $topNode = new DOMNode;
                $topNode = $doc->childNodes->item(0); //assumes that we only care about the first child (there shouldn't ever be any other top-level children)
            }
        ?>
        <h1>Rules for Room <?php print $_GET['room'] ?></h1>

        <form action=<?php print 'submitform.php?room='; print $_GET['room'];?> method="post">
        <fieldset>
            <legend>Hot Count</legend>
            <p>
                <!--this should be updated to go get 'hotCount' based on 'room' -->
                <p>Current value is <?php print $_GET['hotCount']?></p>
                <label>New number of "Too Hot" messages needed to respond:</label>
                <select name="tooHotList">
                    <option value="0" selected="selected">No Change</option>
                    <option value="1">one</option>
                    <option value="2">two</option>
                    <option value="3">three</option>
                    <option value="4">four</option>
                    <option value="5">five</option>
                    <option value="6">six</option>
                    <option value="7">seven</option>
                    <option value="8">eight</option>
                    <option value="9">nine</option>
                    <option value="10">ten</option>
                </select>
            </p>
        </fieldset>
        <fieldset>
            <legend>Cold Count</legend>
            <p>
                <p>Current value is <?php print $_GET['coldCount']?></p>
                <label>New number of "Too Cold" messages needed to respond:</label>
                <select name="tooColdList">
                    <option value="0" selected="selected">No Change</option>
                    <option value="1">one</option>
                    <option value="2">two</option>
                    <option value="3">three</option>
                    <option value="4">four</option>
                    <option value="5">five</option>
                    <option value="6">six</option>
                    <option value="7">seven</option>
                    <option value="8">eight</option>
                    <option value="9">nine</option>
                    <option value="10">ten</option>
                </select>
            </p>
        </fieldset>
        <fieldset>
            <legend>Submit</legend>
            <p>
                <p>Click here to submit all changes on the page</p>
                <input type="submit" />
            </p>
        </fieldset>
        </form>

    </body>
</html>
