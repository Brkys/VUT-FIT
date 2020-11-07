<?php

namespace App\Http\Controllers;
use Illuminate\Support\Facades\DB;

use Illuminate\Http\Request;

class LandsController extends Controller
{
    public function lands(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }

		$lands = DB::select('SELECT Adresa, JmenoFamilie FROM Uzemi LEFT JOIN Familie ON Familie.ID_Familie=Uzemi.Majitel', [1]);

        return view('lands')->with('lands', $lands);
    }

}
