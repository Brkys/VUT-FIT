<?php

namespace App\Http\Controllers;
use Illuminate\Support\Facades\DB;

use Illuminate\Http\Request;

class MembersController extends Controller
{
    public function members(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }

		$members = DB::select("SELECT full_name as 'Jmeno', JmenoFamilie, YEAR(CURDATE()) - YEAR(birth_date) AS 'Vek' FROM users INNER JOIN Familie ON users.familia_id=Familie.ID_Familie", [1]);
        return view('members')->with('members', $members);
    }
}
