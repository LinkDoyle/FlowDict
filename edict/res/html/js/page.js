var noteContent = "";
var noteContentId = "";

function setCurrentWord(word, starLevel) {
  $('#current-word').text(word);
  var stars = $('#word-star-btn button');
  for (var i = 0; i < starLevel; ++i) {
    stars.eq(i).addClass("active").html("<span class='glyphicon glyphicon-star'></span>");
  }
  for (var i = starLevel; i < 5; ++i) {
    stars.eq(i).removeClass("active").html("<span class='glyphicon glyphicon-star-empty'></span>");
  }
}

function addYodao() {
  var content = "正在联网查询...";
  var id = addArticle("有道在线翻译", content);
  $.ajax({
    url: "http://fanyi.youdao.com/openapi.do",
    //keyfrom=orchid&key=1008797533
    data: {
      keyfrom: "f2ec-org",
      key: 1787962561,
      type: "data",
      doctype: "jsonp",
      version: "1.1",
      q: $('#current-word').text()
    },
    dataType: 'jsonp',
    jsonpCallback: 'CallBack',
    success: function(data) {
      var article = $('#' + id);
      article.text("");
      if (data.errorCode == 0) {
        if (data.translation !== undefined) {
          var body = $('<div class="panel-body"></div>');
          var explains = data.translation;
          for (var i in explains) {
            body.append($('<p></p>').text(explains[i]));
          }
          article.append(body);
        }
        if (data.basic !== undefined) {
          var basic = data.basic;
          var ul = $('<ul class="list-group"></ul>');
          ul.append($('<li class="list-group-item"></li>').append($('<h4 class="list-group-item-heading"></h4>').text('基本释义')));
          var phonetic = $('<li class="list-group-item"></li>');
          phonetic.append($('<span></span>').text('英 [' + basic['uk-phonetic'] + ']\t'));
          phonetic.append($('<span></span>').text('美 [' + basic['us-phonetic'] + ']'));
          ul.append(phonetic);
          var explains = basic.explains;
          for (var i in explains) {
            ul.append($('<li class="list-group-item"></li>').text(explains[i]));
          }
          article.append(ul);
        }
        if (data.web !== undefined) {
          var words = data.web;
          var ul = $('<ul class="list-group"></ul>');
          var li = $('<li class="list-group-item"></li>');
          li.append($('<h4 class="list-group-item-heading"></h4>').text('网络释义'));
          ul.append(li);
          for (var i in words) {
            var word = words[i];
            var content = '';
            var li = $('<li class="list-group-item"></li>');
            li.append($('<h5 class="list-group-item-heading"></h5>').text(word.key));
            for (var i in word.value) {
              content += word.value[i] + '; ';
            }
            li.append($('<p class="list-group-item-text"></p>').text(content));
            ul.append(li);
          }
          article.append(ul);
        }
      } else {
        article.text("查询失败，请稍后再试");
      }
      console.log(data);
    }
  });
}

function addWordNote(text) {
  noteContent = text;
  noteContentId = addArticle("我的笔记", marked(text) + '<button type="button" class="btn btn-default" data-toggle="modal" data-target="#noteEditor">编辑</button>');
}

function addArticle(dict_name, text) {
  var article = document.querySelector('#tpl-article').cloneNode(true);
  var container = document.querySelector('#container');
  var index = container.childElementCount;
  article.innerHTML = article.innerHTML.replace(/{{index}}/g, index);
  article.innerHTML = article.innerHTML.replace(/{{name}}/g, dict_name);
  article.innerHTML = article.innerHTML.replace(/{{text}}/g, text);
  var fragment = document.importNode(article.content, true)
  container.appendChild(fragment);
  if (index == 0) $('#article0').collapse('show');
  var a = $('<a></a>').text(dict_name)
    .attr({
      href: '#article_heading' + index,
    })
    .click(function() {
      var article = $(this).attr('href').replace('_heading', '');
      $(article).collapse('show');
    });
  var li = $('<li></li>').append(a);
  $('.sidebar-nav').append(li);
  return $(article.content).find('.panel-collapse').attr('id');
}

function clearArticles() {
  var container = document.querySelector('#container');
  container.innerHTML = "";
  $('.sidebar-nav li:gt(0)').remove();
}

(function() {
  $(document).ready(function() {
    $('[data-toggle="tooltip"]').tooltip();
  });


  function getCurrentWord() {
    return $('#current-word').text();
  }

  function on_star_word(word, level, group) {
    console.log("on_star_word:", { word, level, group });
  }

  function on_unstar_word(word, group) {
    console.log("on_unstar_word:", { word, group });
  }

  function on_modify_star(word) {
    console.log("on_modify_star:", { word });
    $('#starEditor').modal('show');
  }

  function query_star_info(word) {
    return {
      level: 3,
      group: "默认"
    }
  }

  $(".sidebar-toggle").click(function(e) {
    e.preventDefault();
    var span = $('.sidebar-toggle span');
    if ($('span', this).hasClass("glyphicon-remove")) {
      span.addClass('animated fadeOut');
      span.removeClass('glyphicon-remove animated fadeOut');
      span.addClass('glyphicon-menu-right animated fadeIn');
    } else {
      span.addClass('animated fadeOut');
      span.removeClass('glyphicon-menu-right animated fadeOut');
      span.addClass('glyphicon-remove animated fadeIn');
    }
    $("#wrapper").toggleClass("toggled");
  });

  // Star the word
  $('#word-star-btn button').click(function(e) {
    var self = $(this);
    if (!self.nextAll().hasClass('active')) {
      if (self.prev().hasClass('active') && self.hasClass('active')) {
        on_modify_star(getCurrentWord());
        return;
      } else {
        self.toggleClass('active');
        $('span', this).toggleClass('glyphicon-star glyphicon-star-empty');
      }
    }
    self.prevAll().addClass("active").html("<span class='glyphicon glyphicon-star'></span>");
    self.nextAll().removeClass("active").html("<span class='glyphicon glyphicon-star-empty'></span>");
    var starLevel = self.index('.btn');
    if ($('#word-star button').first().hasClass('active')) {
      on_star_word(getCurrentWord(), starLevel + 1, "");
      show_alert("成功添加单词到单词本。");
    } else {
      on_unstar_word(getCurrentWord(), "");
      show_alert("已从单词本中移除单词。");
    };

    function show_alert(text) {
      $('#word-star-alert .alert span:last').text(text);
      var alert = $("#word-star-alert");
      if (!alert.is(':visible')) {
        alert.slideDown('fast').fadeTo(1500, 1, function() {
          alert.slideUp('normal');
        });
      }
    }
  });


  // star Editor
  function starEditor_getList() {
    return [{
      name: "我的单词本",
      checked: true
    }];
  }

  var starEditor_addGroup = function(name, checked) {
    var list = document.querySelector('#tpl-wordbook-list').cloneNode(true);
    var container = document.querySelector('#starEditor .modal-content');
    var index = container.childElementCount;
    list.innerHTML = list.innerHTML.replace(/{{name}}/g, name);
    list.innerHTML = list.innerHTML.replace(/{{checked}}/g, checked ? "check" : "unchecked");
    var fragment = document.importNode(list.content, true);

    // checkboxes
    fragment.querySelector('.word-book-checkbox').onclick = function(e) {
      var self = $(this);
      var span = $('span', this);
      var groupName = self.next('span').text();
      span.toggleClass('glyphicon-unchecked glyphicon-check');
      on_starEditor_checked(getCurrentWord(),
        groupName,
        span.hasClass('glyphicon-check')
      );
    };

    // editname
    fragment.querySelector('.word-book-editname').onclick = function(e) {
      var updataName = function() {
        if (span.val() !== span.text())
          on_starEditor_changeName(span.val(), span.text());
        span.val('');
        span.attr('contentEditable', false);
      }
      var self = $(this);
      var span = $(this).prev('span');
      if ($('span', this).hasClass('glyphicon-pencil')) {
        span.val(span.text());
        span.attr('contentEditable', true);
        span.focus();
        $('#starEditor span[contenteditable]').keydown(function(e) {
          if (e.keyCode === 13) {
            updataName();
            self.toggleClass('glyphicon-pencil glyphicon-ok-circle');
            return false;
          }
        });
      } else {
        updataName();
      }
      $('span', this).toggleClass('glyphicon-pencil glyphicon-ok-circle');
    };

    // insert
    var nodes = document.querySelectorAll('#starEditor .modal-content .list-group-item');
    container.insertBefore(fragment, nodes[nodes.length - 1]);
  }

  function on_starEditor_changeName(oldName, newName) {
    console.log('on_starEditor_changeName:', { oldName, newName });
  }

  function on_starEditor_checked(word, groupName, checked) {
    console.log('on_starEditor_checked:', { word, groupName, checked });
  }

  function on_starEditor_createGroup(groupName) {
    console.log('on_starEditor_createGroup:', { groupName });
  }

  $('#starEditor').on('show.bs.modal', function(e) {
    $('#starEditor .modal-content .list-group-item:not(:last)').remove();

    // generate wordbook list
    var wordbook_list = starEditor_getList();
    for (var i in wordbook_list) {
      starEditor_addGroup(wordbook_list[i].name, wordbook_list[i].checked);
    }
  });

  $('#starEditor .word-book-add').click(function(e) {
    starEditor_addGroup("未命名分组", false);
    on_starEditor_createGroup("未命名分组");
  });

  // Note Editor
  function on_noteEditor_changed(content) {

  }

  $('#noteEditor').on('show.bs.modal', function(e) {
    $('textarea', this).text(noteContent);
  });

  $('#noteEditor .btn:eq(1)').click(function(e) {
    noteContent = $('#noteEditor textarea').val();
    $('#' + noteContentId + ' .panel-body').html(marked(noteContent) + '<button type="button" class="btn btn-default" data-toggle="modal" data-target="#noteEditor">编辑</button>');
    on_noteEditor_changed(noteContent);
  });
})();
