<?php

namespace App\Http\Controllers;

use Illuminate\Support\Facades\DB;

use Illuminate\Http\Request;

use App\Familie;
use App\userModel;
use App\newsModel;
use App\landModel;
use App\Invitation;

class AdminController extends Controller
{
    public function admin() {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 5){
            return redirect('no-permission');
        }
        else 
        {

            $familias = DB::select("SELECT ID_Familie, JmenoFamilie, ID_Dona FROM Familie", [1]);
            $freeUsers = DB::select("SELECT id, full_name FROM users WHERE familia_id IS NULL");


            return view('admin')->with('familias', $familias)->with('freeUsers', $freeUsers);
        }
    }

    public function createNewFamilia(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 5){
            return redirect('no-permission');
        }

        $newFamilia = new Familie();
        $newFamilia->JmenoFamilie = $req->input('familia_name');
        $newFamilia->ID_Dona = $req->input('don_id');
        $newFamilia->save();


        $newDon = userModel::find($req->input('don_id'));
        $newDon->permission = 4;
        $newDon->familia_id = $newFamilia->ID_Familie;
        $newDon->save();

        $familia_name = $newFamilia->JmenoFamilie;
        $don_name = $newDon->full_name;

        $new = new newsModel();
        $new->date = date("Y-m-d H:i:s");
        $new->title = "Nová familie";
        $new->content = "Vznikla familie $familia_name, jejím donem je $don_name";
        $new->save();
        return redirect('admin');
    }

    public function createNewLand(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 5){
            return redirect('no-permission');
        }

        $newLand = new landModel();
        $newLand->Majitel = $req->input('land_owner');
        $newLand->Adresa = $req->input('land_address');
        $newLand->Rozloha = $req->input('land_size');
        $newLand->save();

        $landSize = $newLand->Adresa;

        $new = new newsModel();
        $new->date = date("Y-m-d H:i:s");
        $new->title = "Nové území";
        $new->content = "Vzniklo nové území s adresou $landSize";
        $new->save();
        return redirect('admin');
    }

    public function deleteFamilia(Request $req)
    {
        session_start();
        if(!isset($_SESSION['loggedIn']) || $_SESSION['loggedIn'] !== true){
            return redirect('home')->with('openLogin', true);   
        }
        else if($_SESSION['permission'] !== 5){
            return redirect('no-permission');
        }

        $familiaToDelete = Familie::find($req->input('familia_id'));
        $usersInFamilia =  DB::select("SELECT id FROM users WHERE familia_id = ".$req->input('familia_id'));
        $landsOfFamilia = DB::select("SELECT id FROM Uzemi WHERE familia_id = ".$req->input('familia_id'));
        foreach ($usersInFamilia as $user) {
            $userToUpdate = userModel::find($user->id);
            $userToUpdate->familia_id = NULL;
            $userToUpdate->permission = -1;
            $userToUpdate->save();
        }
        foreach ($landsOfFamilia as $land) {
            $landToChange = landModel::find($land->id);
            $landToChange->Majitel = NULL;
            $landToChange->save();
        }
        $familiaToDelete->ID_Dona = NULL;
        $familiaToDelete->save();
        return redirect('admin');
    }
}