<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;

class TestController extends Controller
{
 
    public function test()
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        return view('test');
    }

}
