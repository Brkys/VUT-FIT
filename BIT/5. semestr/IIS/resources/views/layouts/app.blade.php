<!DOCTYPE html>
<html lang="cz">
<head>
    <title>Mafie - @yield('title')</title>

    <meta charset="utf-8">
    <link rel="stylesheet" type="text/css" href="css/app.css">

    <!-- Bootstrap CSS --> 
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css" integrity="sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO" crossorigin="anonymous">
    @section('head')
</head>
<body background="black">

@php
    if(empty(Session::get('registerInput'))){
        $registerInput = array(
            'fullname' => '',
            'username' => '', 
            'date'     => '',
            'password' => '',
            'email'    => '');
    }
    else{
        $registerInput = Session::get('registerInput');
    }
@endphp
<body>
    @section('navpanel')
    <!-- Navigator panel -->
    <nav class="navbar navbar-expand-lg navbar-dark navbar-fixed-top" id="navbar" style="background-color: #262626">
        <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>

        <div class="collapse navbar-collapse" id="navbarSupportedContent"> 
            <ul class="navbar-nav mr-auto">
                <li class="nav-item {{ Request::segment(1) === 'home' ? 'active' : null }}">
                    <a class="nav-link" href="{{ route('home') }}">Domů <span class="sr-only">(current)</span></a>
               </li>
                <li class="nav-item {{ Request::segment(1) === 'news' ? 'active' : null }}">
                   <a class="nav-link" href=" {{ url('news') }}">Novinky</a>
                </li>
                <li class="nav-item dropdown">
                    <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Přehled</a>
                    <div class="dropdown-menu" aria-labelledby="navbarDropdown">
                        <a class="dropdown-item" href="{{ url('familias') }}">Rodiny</a>
                        <a class="dropdown-item" href="{{ url('members') }}">Členové</a>
                        <a class="dropdown-item" href="{{ url('lands') }}">Území</a>
                        <div class="dropdown-divider"></div>
                        <a class="dropdown-item" href="{{ url('free-users') }}">Volní uživatelé</a>
                        <a class="dropdown-item" href="{{ url('show-alliance') }}">Aliance</a>
                    </div>
                </li>
                <li class="nav-item dropdown">
                    <a class="nav-link dropdown-toggle" href="#" id="navbarDropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Kriminální činnost</a>
                    <div class="dropdown-menu" aria-labelledby="navbarDropdown">
                        <a class="dropdown-item" href="{{ url('show-criminal') }}">Zobrazit</a>
                        <a class="dropdown-item" href="{{ url('assign-criminal') }}">Přiřadit</a>
                        <a class="dropdown-item" href="{{ url('create-criminal') }}">Vytvořit</a>
                    </div>
                </li>
            </ul>
            @if(!isset($_SESSION["loggedIn"]) || $_SESSION["loggedIn"] !== true)
            <span class="navbar-text my-2 my-sm-0 pr-1">
                <a class="btn btn-sm my-2 my-sm-0" href="#" data-toggle="modal" data-target="#registerModal">Registrovat se</a>
            </span>
            <button class="btn btn-outline-success my-2 my-sm-0" type="button" data-toggle="modal" data-target="#loginModal">Příhlásit se</button>
            @else
                @if(!empty($_SESSION['loggedIn']) && $_SESSION['loggedIn'] === true && $_SESSION['permission'] === 5)
                <span class="navbar-text my-2 my-sm-0 pr-1">
                    <a class="btn btn-sm my-2 my-sm-0" href="{{ url('admin') }}">Administrace</a>
                </span>
                @endif

                @if(!empty($_SESSION['loggedIn']) && $_SESSION['loggedIn'] === true && $_SESSION['permission'] === 4)
                <span class="navbar-text my-2 my-sm-0 pr-1">
                    <a class="btn btn-sm my-2 my-sm-0" href="{{ url('familia-editing') }}">Administrace familie</a>
                </span>
                @endif
            <span class="navbar-text my-2 my-sm-0 pr-1">
                <a class="btn btn-sm my-2 my-sm-0" href="{{ url('account') }}">Můj účet</a>
            </span>
            <form action="logout" method="get">
                <button class="btn btn-outline-success my-2 my-sm-0" type="submit">Odhlásit se</button>
            </form>
            @endif
        </div>
    </nav>

    <!-- Modal login --> 
    <div class="modal fade" id="loginModal" tabindex="-1" role="dialog" aria-labeledby="modal_login">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="modal_login">Login</h5>

                    <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
                </div>
                <div class="modal-body" id="modal_body">
                    <form action="login" method="post">

                        <label>Uživatelské jméno</label>
                        <input required type="text" name="username" id="username" placeholder="Toto pole je povinné" class="form-control"/>
                        <br/>
                        <label>Heslo</label>
                        <input required type="password" name="password" id="password" placeholder="Toto pole je povinné" class="form-control" />
                        @csrf
                        <br/>
                        @if ($errors->any() && empty(Session::get('registerNotValid')))
                            <div class="alert alert-danger">
                                <ul>
                                    @foreach ($errors->all() as $error)
                                        <li>{{ $error }}</li>
                                    @endforeach
                                </ul>
                            </div>  
                        @endif
                        <button type="submit" name="login_btn" id="login_btn" class="btn btn-outline-success">Login</button>
                        <button name="register_btn" id="register_btn" class="btn btn-outline-primary" data-dismiss="modal" data-toggle="modal" data-target="#registerModal">Nemáte účet?</button>
                    </form>
                </div>
            </div>
        </div>
    </div>



    <!-- Modal register -->
    <div class="modal fade" id="registerModal" tabindex="-1" role="dialog" aria-labelledby="registerModalLabel" aria-hidden="true">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="registerModalLabel">Register</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
                </div>
                <div class="modal-body">
     
                    <form id="formRegister" class="form-horizontal" role="form" method="post" action="register">
                        @csrf
     
                        <div class="form-group">
                            <label class="col-md-4 control-label">Jméno a příjmení</label>
                                <input required placeholder="Toto pole je povinné" value="{{$registerInput['fullname']}}" type="text" class="form-control" name="fullname">
                        </div>

                        <div class="form-group">
                            <label class="col-md-4 control-label">Datum narození</label>
                                <div class="input-group mb-3">
                                    <div class="input-group-prepend">
                                        <span class="input-group-text">YYYY-MM-DD</span>
                                    </div>
                                    <input  type="text" value="{{$registerInput['date']}}" class="form-control" name="birthdate">
                                </div>
                        </div>


                        <div class="form-group">
                            <label class="col-md-4 control-label">Uživatelské jméno</label>
                                <input required placeholder="Toto pole je povinné" type="text" value="{{$registerInput['username']}}" class="form-control" name="username">
                        </div>
     
                        <div class="form-group">
                            <label class="col-md-4 control-label">E-Mail</label>
                            <div class="input-group mb-3">
                                <div class="input-group-prepend">
                                    <span class="input-group-text">mail@example.com</span>
                                </div>
                                <input type="email" class="form-control" value="{{$registerInput['email']}}" name="email"> 
                            </div>
                        </div>
     
                        <div class="form-group">
                            <label class="col-md-4 control-label">Heslo</label>
                                <input required placeholder="Toto pole je povinné" type="password" class="form-control" value="{{$registerInput['password']}}" name="password">
                        </div>
                        
                        <div class="form-group">
                            <label class="col-md-4 control-label">Zopakovat heslo</label>
                                <input required placeholder="Toto pole je povinné" type="password" class="form-control" value="{{$registerInput['password']}}" name="password_confirmation">
                        </div>

                        @if ($errors->any() && !empty(Session::get('registerNotValid')))
                            <div class="alert alert-danger">
                                <ul>
                                    @foreach ($errors->all() as $error)
                                        <li>{{ $error }}</li>
                                    @endforeach
                                </ul>
                            </div>  
                        @elseif (!empty(Session::get('registerNotValid')) && Session::get('userExists') == true)
                            <div class="alert alert-danger">
                                <ul>
                                    <li>Toto uzivatelske jmeno je jiz zabrane!</li>
                                </ul>
                            </div>
                        @endif
     
                        <div class="form-group">
                            <div class="col-md-6 col-md-offset-4">
                                <button type="submit" class="btn btn-outline-success">
                                    Register
                                </button>
                                <button name="register_btn" id="register_btn" class="btn btn-outline-primary" data-dismiss="modal" data-toggle="modal" data-target="#loginModal">Mám účet</button>
                            </div>
                        </div>
                    </form>                       
     
                </div>
            </div>
        </div>
    </div>
    @show

    <!-- Content of page -->
    @yield('content')

    <!-- Bootstrap javascript -->
    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js" integrity="sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49" crossorigin="anonymous"></script>
    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js" integrity="sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy" crossorigin="anonymous"></script>

@if((!empty(Session::get('openLogin')) && Session::get('openLogin') == true && (!isset($_SESSION["loggedIn"]) || $_SESSION["loggedIn"] !== true)) ||!empty(Session::get('validationFail')) && Session::get('validationFail') == true)
<script>
$(document).ready(function() {
    $("#loginModal").modal('show');
});
</script>
@endif

@if(!empty(Session::get('registerNotValid')) && Session::get('registerNotValid') == true && (!isset($_SESSION["loggedIn"]) || $_SESSION["loggedIn"] !== true)) 
<script>
$(document).ready(function () {
    $("#registerModal").modal('show');
});
</script>
@endif

</body>
</html>