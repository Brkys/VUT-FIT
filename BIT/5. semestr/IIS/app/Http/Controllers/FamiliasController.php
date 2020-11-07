<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\Familie;
use App\userModel;
use App\newsModel;
use Illuminate\Support\Facades\DB;

class FamiliasController extends Controller
{
    public function familias(){
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
		}
		
		$familias = Familie::all();
		$dons = userModel::where('permission', 4)->get();
		$results = array();
    	foreach($familias as $key => $value){
			foreach($dons as $key2 => $value2)
			{
				if($value->ID_Dona == $value2->id){
					array_push($results, array ('ID_Familie' => $value->ID_Familie, 'JmenoFamilie' => $value->JmenoFamilie,'JmenoDona' => $value2->full_name));
				}
			}
		}
		foreach($results as &$result) {
			$members = userModel::where('familia_id', $result['ID_Familie'])->count();
			$result['members'] = $members;
		}
		unset($result);
		
        return view('familias')->with('results', $results);
	}

	public function editFamilia () {
		session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
		}
		if($_SESSION['permission'] != 4) {
			return view('no-permission');
		}

		$usersAndPermissions = DB::select("SELECT id, full_name, permission FROM users WHERE familia_id = ".$_SESSION['familia'], [1]);
		$familiaName = DB::select("SELECT JmenoFamilie FROM Familie WHERE ID_Familie = ".$_SESSION['familia'], [1]);
		$alianceRequests = DB::select("SELECT ID_Familie1 AS PozvankaOd, JmenoFamilie FROM PozvankaDoAliance INNER JOIN Familie WHERE ID_Familie=ID_Familie1 AND ID_Familie2 = ".$_SESSION['familia']);

		return view('familia-editing')->with('users', $usersAndPermissions)->with('familiaName', $familiaName[0]->JmenoFamilie)->with('alianceRequests', $alianceRequests);
	}

	public function promote(Request $req)
	{
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else {
    		if($_SESSION['permission'] != 4) {
				return view('no-permission');
			}   
			$user = userModel::find($req->input('id'));
			$user->permission = $user->permission+1;
			$user->save();

			$name = $user->full_name;

			$news = new newsModel();
			$news->date = date("Y-m-d H:i:s");
            $news->title = "Člen povýšen";
            $news->content = "Člen $name byl povýšen.";
            $news->save();	
        }
		return redirect('familia-editing');

	}

	public function degrade(Request $req)
	{
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else {
			if($_SESSION['permission'] != 4) {
				return view('no-permission');
			}
			$user = userModel::find($req->input('id'));
			$user->permission = $user->permission-1;
			$user->save();

			$name = $user->full_name;

			$news = new newsModel();
			$news->date = date("Y-m-d H:i:s");
            $news->title = "Člen degradován";
            $news->content = "Člen $name byl degradován.";
            $news->save();
        }
		return redirect('familia-editing');

	}

	public function kick(Request $req)
	{
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);
        }
        else {       	
			if($_SESSION['permission'] != 4) {
				return view('no-permission');
			}
			$user = userModel::find($req->input('id'));
			$user->familia_id = NULL;
			$user->permission = -1;
			$user->save();

			$name = $user->full_name;

			$news = new newsModel();
			$news->date = date("Y-m-d H:i:s");
            $news->title = "Člen vyhozen";
            $news->content = "Člen $name byl vyhozen z familie.";
            $news->save();
        }
		return redirect('familia-editing');
	}
}
