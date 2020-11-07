@extends('layouts.app')

@section('title', 'Kriminální činnosti')

@section('navpanel')
	@parent
@endsection

@section('content')
<div class="container" style="padding-top: 100px; width: 80% !important;"> 
    <table class="table table-hover table-dark text-centered" id="filterTable">
        <thead>
            <tr>
                <th scope="col" style="width: 15%;">Území</th>
                <th scope="col" style="width: 15%;">Familie</th>
                <th scope="col" style="width: 40%;">Popis činnosti</th>
                <th scope="col" style="width: 15%;">Datum začátku</th>
                <th scope="col" style="width: 15%;">Datum ukončení</th>
            </tr>
        </thead>
        <tbody>
            @foreach($result as $row)
            <tr>
                <td>{{$row->Adresa}}</td>
                <td>{{$row->JmenoFamilie}}</td>
                <td>{{$row->TypCinnosti}}</td>
                <td>{{$row->DatumZacatku}}</td>
                <td>{{$row->DatumUkonceni}}</td>
            </tr>
            @endforeach
        </tbody>
    </table>
</div>
@endsection