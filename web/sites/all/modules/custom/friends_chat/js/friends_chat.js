/**
 * Created by Corin on 16.02.2018.
 */

jQuery(document).ready(function($) {

    var openChatIDs = [];
    var openChatWindowIDs = [];
    var newMessagesPolled = [];

    var right = 230;
    var maxBoxPerPage = 0;
    var activeBoxID = 0;

    calcMaxBoxPerPage();
    getFriendsList();
    getSettings();


    setTimeout(function () {
        chatPoll();
    }, 500);

    function initChatPoll() {
        setTimeout(function () {
            chatPoll();
            getFriendsList()
        }, 3000);
    }

    function getChatHistory(sender) {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-chat-history';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {sender: sender},
            success: function (data) {
                //console.log(data);
                if (data.status === 'success') {
                    var length = data.messages.length;
                    var chatbox = $('.friends-chatbox[boxid=' + data.sender + ']');
                    chatbox.find('.chatbox-content').html('');

                    for (var i = 0; i < data.messages.length; i++) {

                        var m = data.messages;
                        var date = getFormattedDate(m[i].created_timestamp*1000);

                        chatbox.find('.chatbox-content').append('<div class="' + m[i].type + '" timestamp="' + m[i].created_timestamp + '">' + m[i].message + '<span class="time">' + date + '</span></div>');
                        chatbox.find('.chatbox-content').scrollTop(chatbox.find('.chatbox-content')[0].scrollHeight);

                        if (checkMessagePolled(data.sender, m[i].created_timestamp, newMessagesPolled) === -1 && m[i].read_timestamp === null && m[i].type === 'message-friend') {
                            chatbox.find('.chatbox-head').addClass('blink');
                            newMessagesPolled.push({sender: data.sender, timestamp: m[i].created_timestamp});
                            console.log(newMessagesPolled);
                        }
                    }
                }
            }
        });
    }
    
    function chatPoll() {

        var thisURL = Drupal.settings.basePath + 'friends-chat/chat-poll';
        $.ajax({
            type: 'POST',
            url: thisURL,
            //data: {messagesPolled: JSON.stringify(newMessagesPolled)},
            success: function (data) {
                //console.log(data.messages);

                if (data.status === 'new') {
                    $.each(data.messages, function (i, obj) {

                        if (checkMessagePolled(obj.sender, obj.created_timestamp, newMessagesPolled) === -1) {

                            newMessagesPolled.push({sender: obj.sender, timestamp: obj.created_timestamp});
                            updateMessagesPolled();

                            //console.log('new');
                            if (checkIdExists(obj.sender, openChatIDs) === -1) {
                                var user = {id: obj.sender, name: obj.name};
                                openChatIDs.unshift(user);
                                openChatWindowIDs.unshift(user);

                                $('#friends-chat').prepend(createChatbox(obj.sender, obj.name));

                                getChatHistory(obj.sender);

                                calcMaxBoxPerPage();

                            }

                            var chatbox = $('.friends-chatbox[boxid=' + obj.sender + ']');

                            var date = getFormattedDate(obj.created_timestamp*1000);

                            if (obj.sender !== activeBoxID) {
                                chatbox.find('.chatbox-head').addClass('blink');
                            }
                            chatbox.find('.chatbox-content').append('<div class="message-friend" timestamp="' + obj.created_timestamp + '">' + obj.message + '<span class="time">' + date + '</span></div>');
                            chatbox.find('.chatbox-content').scrollTop(chatbox.find('.chatbox-content')[0].scrollHeight);

                        }

                    });

                    updateOpenChats();


                }
            }
        });

        initChatPoll();
    }

    function getFormattedDate(timestamp) {

        var time;
        if (timestamp === '') {
            time = new Date();
        } else {
            time = new Date(timestamp);
        }

        var day = time.getDate() < 10 ? '0' + time.getDate() : time.getDate();
        var month = (time.getMonth() + 1) < 10 ? '0' + (time.getMonth() + 1) : (time.getMonth() + 1);
        var year = time.getFullYear();
        var hours = time.getHours() < 10 ? '0' + time.getHours() : time.getHours();
        var minutes = time.getMinutes() < 10 ? '0' + time.getMinutes() : time.getMinutes();
        var date = day + '.' + month + '.' + year + ' ' + hours + ':' + minutes;

        return date;
    }

    function updateMessagesPolled() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/update-messages-polled';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {messagesPolled: JSON.stringify(newMessagesPolled)},
            success: function (data) {
            }
        });
    }

    function setReadTimestamp(sender) {
        var thisURL = Drupal.settings.basePath + 'friends-chat/set-read-timestamp';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {sender: sender},
            success: function (data) {
            }
        });
    }
    
    function getSettings() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-settings';

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function (data) {
                if (data.length !== 0) {
                    if (typeof data.openChatIDs !== 'undefined' && data.openChatIDs.length > 0) {
                        openChatIDs = $.parseJSON(data.openChatIDs);
                        openChatWindowIDs = $.parseJSON(data.openChatWindowIDs);

                        $.each(openChatIDs, function (index, data) {
                            $('#friends-chat').append(createChatbox(data.id, data.name));

                            if (checkIdExists(data.id, openChatWindowIDs) === -1) {
                                $('.friends-chatbox[boxid=' + data.id + ']').find('.chatbox-main').hide();
                            }

                            getChatHistory(data.id);
                            adjustChatboxRight();
                        });
                    }

                    if (typeof data.messagesPolled !== 'undefined' && data.messagesPolled.length > 0) {
                        newMessagesPolled = $.parseJSON(data.messagesPolled);
                    }

                }
            }
        });
    }
    function getFriendsList() {
        var thisURL = Drupal.settings.basePath + 'friends-chat/get-friends-list';

        $.ajax({
            type: 'GET',
            url: thisURL,
            success: function(data) {
                $('.friends-list .list-content').html('');

                if (data['status'] === 'error') {
                    $('.friends-list .list-content').append(data['message']);
                }

                if (data['status'] === 'success') {
                    var count = 0;

                    $.each(data['users'], function (key, val) {
                        var html =
                            '<div class="friend">' +
                            '<div class="status ' + val.status + '"></div>' +
                            '<div class="name"><a href="#" class="' + key + '">' + val.name + '</a></div>' +
                            '</div>';

                        $('.friends-list .list-content').append(html);

                        if (checkIdExists(key, openChatIDs) !== -1) {
                            $('friends-chatbox[boxid=' + key + ']').find('.status')
                        }

                        if ($('.friends-chatbox[boxid=' + key + ']').length > 0) {
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').removeClass('online');
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').removeClass('offline');
                            $('.friends-chatbox[boxid=' + key + ']').find('.status').addClass(val.status);
                        }
                        count++;
                    });

                    $('.friends-list .list-head div').html('Chat (' + count + ')')
                }

            },
        });
    }

    $(document).delegate('.chatbox-content, .chatbox-input', 'click', function (e) {
        var chatbox = $(this).parents('.friends-chatbox');
        chatbox.find('.chatbox-head').removeClass('blink');

        activeBoxID = chatbox.attr('boxid');

        clearMessagesPolled(activeBoxID);
        setReadTimestamp(activeBoxID);
    });

    $(document).delegate('body', 'click', function (e) {
        if ($(e.target).parents('.friends-chatbox[boxid=' + activeBoxID + ']').length === 0) {
            activeBoxID = 0;
        }

    });

    $(document).delegate('.friends-list .friend', 'click', function (e) {
        e.preventDefault();

        var boxID = $(this).find('a').attr('class');
        var userName = $(this).find('a').html();

        var user = {id: boxID, name: userName};

        if (checkIdExists(boxID, openChatIDs) !== -1) {
            openChatIDs.splice(checkIdExists(boxID, openChatIDs), 1);
            openChatWindowIDs.splice(checkIdExists(boxID, openChatWindowIDs), 1);

            openChatIDs.unshift(user);
            openChatWindowIDs.unshift(user);

            var element = $('.friends-chatbox[boxid=' + boxID + ']');

            $('.friends-chatbox[boxid=' + boxID + ']').remove();

            $('#friends-chat').prepend(element);
            $('.friends-chatbox[boxid=' + boxID + ']').find('.chatbox-main').show();

        }

        if (checkIdExists(boxID, openChatIDs) === -1 && checkIdExists(boxID, openChatWindowIDs) === -1) {

            $('#friends-chat').prepend(createChatbox(boxID, userName));

            openChatIDs.unshift(user);
            openChatWindowIDs.unshift(user);

        }

        getChatHistory(boxID);
        adjustChatboxRight();

        $('.friends-chatbox[boxid=' + boxID + '] .chatbox-input .editor').focus();

        updateOpenChats();

    });

    function checkIdExists(id, list) {
        var i;
        for (i = 0; i < list.length; i++) {
            if (list[i].id === id) {
                return i;
            }
        }

        return -1;
    }

    function checkMessagePolled(sender, timestamp, list) {
        var i;
        for (i = 0; i < list.length; i++) {
            if (list[i].sender === sender && list[i].timestamp === timestamp) {
                return i;
            }
        }

        return -1;
    }

    function clearMessagesPolled(boxid) {

        newMessagesPolled = $.grep(newMessagesPolled, function (el, i) {
            if (el.sender === boxid) {
                return false;
            }

            return true;
        });

        updateMessagesPolled();
    }

    function createChatbox(boxID, userName) {
        var boxHTML =
            '<div class="friends-chatbox" boxID="' + boxID + '">' +
            '<div class="panel">' +
            '<div class="chatbox-head"><div>' + userName + '</div></div>' +
            '<div class="chatbox-main">' +
            '<div class="chatbox-title">' +
            '<div class="status"></div>' +
            '<span class="close">x</span>' +
            userName +
            '</div>' +
            '<div class="chatbox-content">' +

            '</div>' +
            '<div class="chatbox-input">' +
            '<div class="editor" contenteditable="true"></div> ' +
            '</div>' +
            '</div>' +
            '</div>' +
            '</div>';

        return boxHTML;
    }
    function updateOpenChats() {

        var thisURL = Drupal.settings.basePath + 'friends-chat/update-open-chats';

        $.ajax({
            type: 'POST',
            url: thisURL,
            data: {openChatIDs: JSON.stringify(openChatIDs), openChatWindowIDs: JSON.stringify(openChatWindowIDs)},
            success: function (data) {
            }
        });
    }

    function adjustChatboxRight () {
        right = 230;
        $('.friends-chatbox').each(function (index) {
            $(this).css('right', right);
            right += 250;

            if (index >= maxBoxPerPage) {
                $(this).hide();
            } else {
                $(this).show();
            }
        });
    }

    function calcMaxBoxPerPage () {
        maxBoxPerPage = Math.floor($(window).width() / 220) - 1;
        adjustChatboxRight();
    }

    $(document).delegate('.chatbox-input .editor', 'keydown', function (e) {
        if (e.keyCode === 13 && event.shiftKey === false) {
            var message = $(this).html();
            message = message.replace(/^(<br\s*\/?>)*|(<br\s*\/?>)*$/ig, '');
            if (message.length > 0) {
                var request = {
                    'receiver': $(this).parents('.friends-chatbox').attr('boxid'),
                    'message': message
                };

                var thisURL = Drupal.settings.basePath + 'friends-chat/save-message';
                $.ajax({
                    type: 'POST',
                    url: thisURL,
                    data: {content: JSON.stringify(request)},
                    success: function (data) {

                    }
                });

                var date = getFormattedDate('');

                var chatboxContent = $(this).parents('.chatbox-main').find('.chatbox-content');
                chatboxContent.append('<div class="message-me">' + message + '<span class="time">' + date + '</span></div>');
                chatboxContent.scrollTop(chatboxContent[0].scrollHeight);

            }

            $(this).html('');
            return false;

        }
    });

    $(document).delegate('.chatbox-main .chatbox-title .close', 'click', function (e) {
        e.stopPropagation();

        var box = $(this).parents('.friends-chatbox');
        openChatIDs.splice(checkIdExists(box.attr('boxid'), openChatIDs), 1);
        openChatWindowIDs.splice(checkIdExists(box.attr('boxid'), openChatWindowIDs), 1);

        box.remove();
        adjustChatboxRight();
        updateOpenChats();
    });

    $(document).delegate('.friends-list .list-head, .friends-list .list-title', 'click', function (e) {
        $(this).parents('.friends-list').find('.list-main').toggle();
    });

    $(document).delegate('.friends-chatbox .chatbox-head, .friends-chatbox .chatbox-title', 'click', function (e) {
        var box = $(this).parents('.friends-chatbox');

        var boxID = box.attr('boxid');
        var userName = $('.friend a[class=' + boxID + ']').html();
        var user = {id: boxID, name: userName};

        if (box.find('.chatbox-main').is(':hidden')) {
            openChatWindowIDs.push(user);
            box.find('.chatbox-main').show();
        } else {
            openChatWindowIDs.splice(checkIdExists(box.attr('boxid'), openChatWindowIDs), 1);
            box.find('.chatbox-main').hide();
        }

        updateOpenChats();

    });

    $(window).resize(function() {
        calcMaxBoxPerPage();
    });
});
